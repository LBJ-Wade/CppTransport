//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "dq_basic_unrolled.h"

#include "transport-runtime-api/repository/repository_creation_key.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

const double mass  = 1E-5 * M_Planck;
const double m_phi = 9.0 * mass;
const double m_chi = 1.0 * mass;

const double phi_init = 10;
const double chi_init = 12.9;

//const double phi_init = 9.3981343686532135;
//const double chi_init = 12.890177463729746;
//const double dphi_init = -0.20654194543752599;
//const double dchi_init = -0.003472577153076918;

// ****************************************************************************


// filter to determine which time values are included on time-series plots - we just use them all
bool timeseries_filter(const transport::derived_data::filter::time_filter_data& data)
  {
    return (true); // plot all points
  }


// filter to determine which 2pf kconfig values are included on wavenumber-series plots.
bool twopf_kseries_filter(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
  {
    return (true); // plot all values of k
  }


// filter to determine which 2pf kconfig values are included on time-series plots.
// to cut down the sample, here I use only the largest
bool twopf_kconfig_filter(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
  {
    return (data.max); // plot only the largest k
  }


// filter to determine which 3pf kconfig values are included on plots.
// to cut down the sample, here I use only the largest
bool threepf_kconfig_filter(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
  {
    return (data.kt_max); // plot only the largest k_t
  }


// filter for near-equilateral 3pf k configurations - pick the largest and smallest kt
bool threepf_kconfig_equilateral(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
  {
    return (fabs(data.alpha) < 0.01 && fabs(data.beta - (1.0 / 3.0)) < 0.01 && (data.kt_max || data.kt_min));
  }


// filter for near-squeezed 3pf k-configurations
bool threepf_kconfig_near_squeezed(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
  {
    return (fabs(data.beta) >= 0.95);
  }


// filter for most-squeezed 3pf k-configuration
bool threepf_kconfig_most_squeezed(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
  {
    return (data.beta_max);  // plot only the largest beta
  }


// filter for most-equilateral 3pf k-configuration
bool threepf_kconfig_most_equilateral(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
  {
    return (fabs(data.alpha) < 0.01 && fabs(data.beta - (1.0 / 3.0)) < 0.01 && data.kt_max);
  }


// filter for all equilateral configurations
bool all_equilateral(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
  {
    return (fabs(data.alpha) < 0.01 & fabs(data.beta - (1.0 / 3.0)) < 0.01);
  }


// filter for near-squeezed 3pf k-configurations
bool all_squeezed(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
  {
    return (fabs(data.beta) > 0.85);
  }


// filter for time configurations
bool time_config_filter(const transport::derived_data::filter::time_filter_data& data)
  {
    return (data.max);
  }


int main(int argc, char* argv[])
  {
    if(argc != 2)
      {
        std::cerr << "makerepo: Too few arguments. Expected repository name" << std::endl;
        exit(EXIT_FAILURE);
      }

    transport::repository_creation_key key;

    transport::json_repository<double>* repo = transport::repository_factory<double>(argv[1], key);

    // set up an instance of a manager
    transport::task_manager<double>* mgr = new transport::task_manager<double>(0, nullptr, repo);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    transport::dquad_basic<double>* model = new transport::dquad_basic<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params = { m_phi, m_chi };
    transport::parameters<double> params(M_Planck, init_params, model);

//    const std::vector<double> init_values = { phi_init, chi_init, dphi_init, dchi_init };
  const std::vector<double> init_values = { phi_init, chi_init };

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 8.0;  // horizon-crossing occurs at 7 e-folds from init_values
    const double Npre   = 5.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 55.0; // how many e-folds to integrate after horizon crossing

//    // set up initial conditions
    transport::initial_conditions<double> ics("dquad-1", model, params, init_values, Ninit, Ncross, Npre);

//    transport::initial_conditions<double> ics("dquad-1", model, params, init_values, 3.0, 5.0);

    const unsigned int t_samples = 1000;       // record 5000 samples - enough to find a good stepsize

    transport::stepping_range<double> times(Ncross-Npre, Nmax + Ncross, t_samples, transport::logarithmic_bottom_stepping);
//    transport::stepping_range<double> times(3.0, 63.0, t_samples, transport::logarithmic_bottom_stepping);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double       kmin      = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double       kmax      = exp(3.0);   // end with the mode which exits the horizon at N=N*+3
    const unsigned int k_samples = 20;          // number of k-points

    struct ThreepfStoragePolicy
      {
      public:
        bool operator()(const transport::threepf_kconfig& data)
          {
            return (true);
          }
      };

    // have to choose linear spacing if we want the task to be fNL-integrable
    transport::stepping_range<double> ks(kmin, kmax, k_samples, transport::linear_stepping);

    // construct a threepf task and a paired zeta task
    transport::threepf_cubic_task<double> tk3("dquad.threepf-1", ics, times, ks, ThreepfStoragePolicy());
    transport::zeta_threepf_task<double>  ztk3("dquad.threepf-1.zeta", tk3);
    ztk3.set_paired(true);

    // construct a twopf task
    transport::twopf_task<double>      tk2("dquad.twopf-1", ics, times, ks);
    transport::zeta_twopf_task<double> ztk2("dquad.twopf-1.zeta", tk2);
    ztk2.set_paired(true);

    // construct some derived data products; first, simply plots of the background

    transport::index_selector<1> bg_sel(model->get_N_fields());
    bg_sel.all();

    transport::derived_data::background_time_series<double> tk2_bg(tk2, bg_sel, transport::derived_data::filter::time_filter(timeseries_filter));

    transport::derived_data::background_time_series<double> tk3_bg(tk3, bg_sel, transport::derived_data::filter::time_filter(timeseries_filter));

    transport::derived_data::time_series_plot<double> tk2_bg_plot("dquad.twopf-1.background", "background.pdf");
    tk2_bg_plot.add_line(tk2_bg);
    tk2_bg_plot.set_title_text("Background fields");

    transport::derived_data::time_series_plot<double> tk3_bg_plot("dquad.threepf-1.background", "background.pdf");
    tk3_bg_plot.add_line(tk3_bg);
    tk3_bg_plot.set_title_text("Background fields");

    // plots of some components of the twopf

    transport::index_selector<2> twopf_fields(model->get_N_fields());
    transport::index_selector<2> twopf_cross(model->get_N_fields());
    twopf_fields.none();
    twopf_cross.none();

    // field-field correlations
    std::array<unsigned int, 2> index_set_a = { 0, 0 };
    std::array<unsigned int, 2> index_set_b = { 0, 1 };
    std::array<unsigned int, 2> index_set_c = { 1, 1 };

    // momenta-field correlations; the imaginary 2pf only has these cross-terms
    std::array<unsigned int, 2> index_set_d = { 2, 0 };
    std::array<unsigned int, 2> index_set_e = { 2, 1 };
    std::array<unsigned int, 2> index_set_f = { 3, 0 };
    std::array<unsigned int, 2> index_set_g = { 3, 1 };

    twopf_fields.set_on(index_set_a);
    twopf_fields.set_on(index_set_b);
    twopf_fields.set_on(index_set_c);

    twopf_cross.set_on(index_set_d);
    twopf_cross.set_on(index_set_e);
    twopf_cross.set_on(index_set_f);
    twopf_cross.set_on(index_set_g);

    transport::derived_data::twopf_time_series<double> tk2_twopf_real_group(tk2, twopf_fields,
                                                                            transport::derived_data::filter::time_filter(timeseries_filter),
                                                                            transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk2_twopf_real_group.set_klabel_meaning(transport::derived_data::conventional);

    transport::derived_data::twopf_time_series<double> tk2_twopf_imag_group(tk2, twopf_cross,
                                                                            transport::derived_data::filter::time_filter(timeseries_filter),
                                                                            transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk2_twopf_imag_group.set_klabel_meaning(transport::derived_data::conventional);
    tk2_twopf_imag_group.set_type(transport::derived_data::imaginary);

    transport::derived_data::twopf_time_series<double> tk3_twopf_real_group(tk3, twopf_fields,
                                                                            transport::derived_data::filter::time_filter(timeseries_filter),
                                                                            transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk3_twopf_real_group.set_klabel_meaning(transport::derived_data::conventional);

    transport::derived_data::twopf_time_series<double> tk3_twopf_imag_group(tk3, twopf_cross,
                                                                            transport::derived_data::filter::time_filter(timeseries_filter),
                                                                            transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk3_twopf_imag_group.set_klabel_meaning(transport::derived_data::conventional);
    tk3_twopf_imag_group.set_type(transport::derived_data::imaginary);

    transport::derived_data::twopf_wavenumber_series<double> tk3_twopf_real_kgp(tk3, twopf_fields,
                                                                                transport::derived_data::filter::time_filter(time_config_filter),
                                                                                transport::derived_data::filter::twopf_kconfig_filter(twopf_kseries_filter));
    tk3_twopf_real_kgp.set_klabel_meaning(transport::derived_data::conventional);

    transport::derived_data::twopf_wavenumber_series<double> tk3_twopf_imag_kgp(tk3, twopf_cross,
                                                                                transport::derived_data::filter::time_filter(time_config_filter),
                                                                                transport::derived_data::filter::twopf_kconfig_filter(twopf_kseries_filter));
    tk3_twopf_imag_kgp.set_klabel_meaning(transport::derived_data::conventional);
    tk3_twopf_imag_kgp.set_type(transport::derived_data::imaginary);

    transport::derived_data::time_series_plot<double> tk2_twopf_real_plot("dquad.twopf-1.twopf-real", "twopf-real.pdf");
    tk2_twopf_real_plot.add_line(tk2_twopf_real_group);
    tk2_twopf_real_plot.set_title_text("Real two-point function");
    tk2_twopf_real_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_plot<double> tk2_twopf_imag_plot("dquad.twopf-1.twopf-imag", "twopf-imag.pdf");
    tk2_twopf_imag_plot.add_line(tk2_twopf_imag_group);
    tk2_twopf_imag_plot.set_title_text("Imaginary two-point function");
    tk2_twopf_imag_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_plot<double> tk2_twopf_total_plot("dquad.twopf-1.twopf-total", "twopf-total.pdf");
    tk2_twopf_total_plot.add_line(tk2_twopf_real_group);
    tk2_twopf_total_plot.add_line(tk2_twopf_imag_group);
    tk2_twopf_total_plot.set_title_text("Two-point function");
    tk2_twopf_total_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_real_plot("dquad.threepf-1.twopf-real", "twopf-real.pdf");
    tk3_twopf_real_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_real_plot.set_title_text("Real two-point function");
    tk3_twopf_real_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_imag_plot("dquad.threepf-1.twopf-imag", "twopf-imag.pdf");
    tk3_twopf_imag_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_imag_plot.set_title_text("Imaginary two-point function");
    tk3_twopf_imag_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_total_plot("dquad.threepf-1.twopf-total", "twopf-total.pdf");
    tk3_twopf_total_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_total_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_total_plot.set_title_text("Two-point function");
    tk3_twopf_total_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::wavenumber_series_plot<double> tk3_twopf_real_spec("dquad.threepf-1.twopf-re-spec", "twopf-re-spec.pdf");
    tk3_twopf_real_spec.add_line(tk3_twopf_real_kgp);
    tk3_twopf_real_spec.set_log_x(true);
    tk3_twopf_real_spec.set_title_text("Real two-point function");

    transport::derived_data::wavenumber_series_plot<double> tk3_twopf_imag_spec("dquad.threepf-1.twopf-im-spec", "twopf-im-spec.pdf");
    tk3_twopf_imag_spec.add_line(tk3_twopf_imag_kgp);
    tk3_twopf_imag_spec.set_log_x(true);
    tk3_twopf_imag_spec.set_title_text("Imaginary two-point function");

    transport::derived_data::wavenumber_series_plot<double> tk3_twopf_total_spec("dquad.threepf-1.twopf-tot-spec", "twopf-tot-spec.pdf");
    tk3_twopf_total_spec.add_line(tk3_twopf_real_kgp);
    tk3_twopf_total_spec.add_line(tk3_twopf_imag_kgp);
    tk3_twopf_total_spec.set_log_x(true);
    tk3_twopf_total_spec.set_title_text("Two-point function");

    transport::derived_data::wavenumber_series_table<double> tk3_twopf_total_tab("dquad.threepf-1.twopf-tot-spec-table", "twopf-tot-spec-table.txt");
    tk3_twopf_total_tab.add_line(tk3_twopf_real_kgp);
    tk3_twopf_total_tab.add_line(tk3_twopf_imag_kgp);


    // plots of some components of the threepf
    transport::index_selector<3> threepf_fields(model->get_N_fields());
    transport::index_selector<3> threepf_momenta(model->get_N_fields());
    threepf_fields.none();
    threepf_momenta.none();

    std::array<unsigned int, 3> sq_set_a = { 0, 0, 0 };
    std::array<unsigned int, 3> sq_set_b = { 0, 1, 0 };
    std::array<unsigned int, 3> sq_set_c = { 1, 1, 0 };
    std::array<unsigned int, 3> sq_set_d = { 0, 0, 1 };
    std::array<unsigned int, 3> sq_set_e = { 0, 1, 1 };
    std::array<unsigned int, 3> sq_set_f = { 1, 1, 1 };
    threepf_fields.set_on(sq_set_a);
    threepf_fields.set_on(sq_set_b);
    threepf_fields.set_on(sq_set_c);
    threepf_fields.set_on(sq_set_d);
    threepf_fields.set_on(sq_set_e);
    threepf_fields.set_on(sq_set_f);

    std::array<unsigned int, 3> sq_set_g = { 2, 2, 2 };
    std::array<unsigned int, 3> sq_set_h = { 2, 3, 2 };
    std::array<unsigned int, 3> sq_set_i = { 3, 3, 2 };
    std::array<unsigned int, 3> sq_set_j = { 2, 2, 3 };
    std::array<unsigned int, 3> sq_set_k = { 2, 3, 3 };
    std::array<unsigned int, 3> sq_set_l = { 3, 3, 3 };
    threepf_momenta.set_on(sq_set_g);
    threepf_momenta.set_on(sq_set_h);
    threepf_momenta.set_on(sq_set_i);
    threepf_momenta.set_on(sq_set_j);
    threepf_momenta.set_on(sq_set_k);
    threepf_momenta.set_on(sq_set_l);

    // THREEPF FIELDS
    transport::derived_data::threepf_time_series<double> tk3_threepf_fields_equi(tk3, threepf_fields,
                                                                                 transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                 transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_equilateral));
    tk3_threepf_fields_equi.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_fields_equi.set_dot_meaning(transport::derived_data::derivatives);

    transport::derived_data::time_series_plot<double> tk3_threepf_field_equi_plot("dquad.threepf-1.f-equi", "f-equi.pdf");
    tk3_threepf_field_equi_plot.add_line(tk3_threepf_fields_equi);
    tk3_threepf_field_equi_plot.set_title_text("Three-point function");
    tk3_threepf_field_equi_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_field_equi_table("dquad.threepf-1.f-equi.table", "f-equi-table.txt");
    tk3_threepf_field_equi_table.add_line(tk3_threepf_fields_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_fields_sq(tk3, threepf_fields,
                                                                               transport::derived_data::filter::time_filter(timeseries_filter),
                                                                               transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_squeezed));
    tk3_threepf_fields_sq.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_fields_sq.set_dot_meaning(transport::derived_data::derivatives);
    tk3_threepf_fields_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_field_sq_plot("dquad.threepf-1.f-sq", "f-sq.pdf");
    tk3_threepf_field_sq_plot.add_line(tk3_threepf_fields_sq);
    tk3_threepf_field_sq_plot.set_title_text("Three-point function");
    tk3_threepf_field_sq_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_field_sq_table("dquad.threepf-1.f-sq.table", "f-sq-table.txt");
    tk3_threepf_field_sq_table.add_line(tk3_threepf_fields_sq);


    // THREEPF DERIVATIVES
    transport::derived_data::threepf_time_series<double> tk3_threepf_deriv_equi(tk3, threepf_momenta,
                                                                                transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_equilateral));
    tk3_threepf_deriv_equi.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_deriv_equi.set_dot_meaning(transport::derived_data::derivatives);

    transport::derived_data::time_series_plot<double> tk3_threepf_deriv_equi_plot("dquad.threepf-1.d-equi", "d-equi.pdf");
    tk3_threepf_deriv_equi_plot.add_line(tk3_threepf_deriv_equi);
    tk3_threepf_deriv_equi_plot.set_title_text("Three-point function");
    tk3_threepf_deriv_equi_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_deriv_equi_table("dquad.threepf-1.d-equi.table", "d-equi-table.txt");
    tk3_threepf_deriv_equi_table.add_line(tk3_threepf_deriv_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_deriv_sq(tk3, threepf_momenta,
                                                                              transport::derived_data::filter::time_filter(timeseries_filter),
                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_squeezed));
    tk3_threepf_deriv_sq.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_deriv_sq.set_dot_meaning(transport::derived_data::derivatives);
    tk3_threepf_deriv_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_deriv_sq_plot("dquad.threepf-1.d-sq", "d-sq.pdf");
    tk3_threepf_deriv_sq_plot.add_line(tk3_threepf_deriv_sq);
    tk3_threepf_deriv_sq_plot.set_title_text("Three-point function");
    tk3_threepf_deriv_sq_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_deriv_sq_table("dquad.threepf-1.d-sq.table", "d-sq-table.txt");
    tk3_threepf_deriv_sq_table.add_line(tk3_threepf_deriv_sq);

    // THREEPF MOMENTA
    transport::derived_data::threepf_time_series<double> tk3_threepf_mma_equi(tk3, threepf_momenta,
                                                                              transport::derived_data::filter::time_filter(timeseries_filter),
                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_equilateral));
    tk3_threepf_mma_equi.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_mma_equi.set_dot_meaning(transport::derived_data::momenta);

    transport::derived_data::time_series_plot<double> tk3_threepf_mma_equi_plot("dquad.threepf-1.m-equi", "m-equi.pdf");
    tk3_threepf_mma_equi_plot.add_line(tk3_threepf_mma_equi);
    tk3_threepf_mma_equi_plot.set_title_text("Three-point function");
    tk3_threepf_mma_equi_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_mma_equi_table("dquad.threepf-1.m-equi.table", "m-equi-table.txt");
    tk3_threepf_mma_equi_table.add_line(tk3_threepf_mma_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_mma_sq(tk3, threepf_momenta,
                                                                            transport::derived_data::filter::time_filter(timeseries_filter),
                                                                            transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_squeezed));
    tk3_threepf_mma_sq.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_mma_sq.set_dot_meaning(transport::derived_data::momenta);
    tk3_threepf_mma_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_mma_sq_plot("dquad.threepf-1.m-sq", "m-sq.pdf");
    tk3_threepf_mma_sq_plot.add_line(tk3_threepf_mma_sq);
    tk3_threepf_mma_sq_plot.set_title_text("Three-point function");
    tk3_threepf_mma_sq_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_mma_sq_table("dquad.threepf-1.m-sq.table", "m-sq-table.txt");
    tk3_threepf_mma_sq_table.add_line(tk3_threepf_mma_sq);


    transport::derived_data::time_series_plot<double> tk3_mixed_plot("dquad.threepf-1.mixed", "mixed.pdf");

    tk3_mixed_plot.add_line(tk3_threepf_fields_equi);
    tk3_mixed_plot.add_line(tk3_twopf_real_group);
    tk3_mixed_plot.set_title_text("Two- and three-point functions");
    tk3_mixed_plot.set_legend_position(transport::derived_data::bottom_left);

    // pick out the shift between derivative and momenta 3pfs
    transport::index_selector<3> threepf_mmta(model->get_N_fields());
    threepf_mmta.none();
    std::array<unsigned int, 3> sq_mmta_a = { 2, 0, 0 };
    std::array<unsigned int, 3> sq_mmta_b = { 3, 0, 0 };
    threepf_mmta.set_on(sq_mmta_a);
    threepf_mmta.set_on(sq_mmta_b);

    transport::derived_data::threepf_time_series<double> tk3_threepf_derivs(tk3, threepf_mmta,
                                                                            transport::derived_data::filter::time_filter(timeseries_filter),
                                                                            transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_filter));
    tk3_threepf_derivs.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_derivs.set_dot_meaning(transport::derived_data::derivatives);
    tk3_threepf_derivs.set_use_alpha_label(true);
    tk3_threepf_derivs.set_use_beta_label(true);

    transport::derived_data::threepf_time_series<double> tk3_threepf_momenta(tk3, threepf_mmta,
                                                                             transport::derived_data::filter::time_filter(timeseries_filter),
                                                                             transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_filter));
    tk3_threepf_momenta.set_klabel_meaning(transport::derived_data::comoving);
    tk3_threepf_momenta.set_dot_meaning(transport::derived_data::momenta);
    tk3_threepf_momenta.set_use_alpha_label(true);
    tk3_threepf_momenta.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_check_shift = transport::derived_data::time_series_plot<double>("dquad.threepf-1.checkshift", "checkshift.pdf");

    tk3_check_shift.add_line(tk3_threepf_derivs);
    tk3_check_shift.add_line(tk3_threepf_momenta);
    tk3_check_shift.set_title_text("Comparison of derivative and momenta 3pf");
    tk3_check_shift.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::time_series_table<double> tk3_check_shift_table("dquad.threepf-1.checkshift.table", "checkshift-table.txt");

    tk3_check_shift_table.add_line(tk3_threepf_derivs);
    tk3_check_shift_table.add_line(tk3_threepf_momenta);

    transport::derived_data::threepf_wavenumber_series<double> tk3_threepf_equi_kgp(tk3, threepf_fields,
                                                                                    transport::derived_data::filter::time_filter(time_config_filter),
                                                                                    transport::derived_data::filter::threepf_kconfig_filter(all_equilateral));
    tk3_threepf_equi_kgp.set_dot_meaning(transport::derived_data::derivatives);
    tk3_threepf_equi_kgp.set_klabel_meaning(transport::derived_data::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_threepf_equi_spec("dquad.threepf-1.threepf-equi-spec", "threepf-equi-spec.pdf");
    tk3_threepf_equi_spec.add_line(tk3_threepf_equi_kgp);
    tk3_threepf_equi_spec.set_log_x(true);
    tk3_threepf_equi_spec.set_title_text("Spectrum of 3pf in equilateral configurations");

    transport::derived_data::wavenumber_series_table<double> tk3_threepf_equi_spec_tab("dquad.threepf-1.threepf-equi-spec-tab", "threepf-equi-spec-tab.txt");
    tk3_threepf_equi_spec_tab.add_line(tk3_threepf_equi_kgp);

    // check the zeta twopf
    transport::derived_data::zeta_twopf_time_series<double> tk3_zeta_twopf_group(ztk3,
                                                                                 transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                 transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));

    transport::derived_data::time_series_plot<double> tk3_zeta_twopf("dquad.threepf-1.zeta-twopf", "zeta-twopf.pdf");

    tk3_zeta_twopf.add_line(tk3_zeta_twopf_group);
    tk3_zeta_twopf.add_line(tk3_twopf_real_group);
    tk3_zeta_twopf.set_title_text("Comparison of $\\zeta$ and field 2pfs");
    tk3_zeta_twopf.set_legend_position(transport::derived_data::bottom_left);

    // check the zeta threepf
    transport::derived_data::zeta_threepf_time_series<double> tk3_zeta_equi_group(ztk3,
                                                                                  transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                  transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_equilateral));
    tk3_zeta_equi_group.set_klabel_meaning(transport::derived_data::comoving);
    tk3_zeta_equi_group.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_equi("dquad.threepf-1.zeta-equi", "zeta-equi.pdf");
    tk3_zeta_equi.add_line(tk3_zeta_equi_group);
    tk3_zeta_equi.set_title_text("3pf of $\\zeta$ near equilateral configurations");

    // set up a table too
    transport::derived_data::time_series_table<double> tk3_zeta_equi_table("dquad.threepf-1.zeta-equi.table", "zeta-equi-table.txt");
    tk3_zeta_equi_table.add_line(tk3_zeta_equi_group);

    transport::derived_data::zeta_threepf_time_series<double> tk3_zeta_sq_group(ztk3,
                                                                                transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_near_squeezed));
    tk3_zeta_sq_group.set_klabel_meaning(transport::derived_data::comoving);
    tk3_zeta_sq_group.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_sq("dquad.threepf-1.zeta-sq", "zeta-sq.pdf");
    tk3_zeta_sq.add_line(tk3_zeta_sq_group);
    tk3_zeta_sq.set_title_text("3pf of $\\zeta$ near squeezed configurations");

    // set up a table too
    transport::derived_data::time_series_table<double> tk3_zeta_sq_table("dquad.threepf-1.zeta-sq.table", "zeta-sq-table.txt");
    tk3_zeta_sq_table.add_line(tk3_zeta_sq_group);

    // compute the reduced bispectrum in a few squeezed configurations
    transport::derived_data::zeta_reduced_bispectrum_time_series<double> tk3_zeta_redbsp(ztk3,
                                                                                         transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                         transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_near_squeezed));
    tk3_zeta_redbsp.set_klabel_meaning(transport::derived_data::comoving);
    tk3_zeta_redbsp.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_redbsp("dquad.threepf-1.redbsp-sq", "redbsp-sq.pdf");
    tk3_redbsp.set_log_y(false);
    tk3_redbsp.set_abs_y(false);
    tk3_redbsp.add_line(tk3_zeta_redbsp);
    tk3_redbsp.set_legend_position(transport::derived_data::bottom_right);
    tk3_redbsp.set_title_text("Reduced bispectrum near squeezed configurations");

    transport::derived_data::time_series_table<double> tk3_redbsp_table("dquad.threepf-1.redbsp-dq.table", "redbsp-sq-table.txt");
    tk3_redbsp_table.add_line(tk3_zeta_redbsp);

    transport::derived_data::zeta_twopf_wavenumber_series<double> tk3_zeta_2spec(ztk3,
                                                                                 transport::derived_data::filter::time_filter(time_config_filter),
                                                                                 transport::derived_data::filter::twopf_kconfig_filter(twopf_kseries_filter));
    tk3_zeta_2spec.set_klabel_meaning(transport::derived_data::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_zeta_2spec_plot("dquad.threepf-1.zeta-2pec", "zeta-2spec.pdf");
    tk3_zeta_2spec_plot.add_line(tk3_zeta_2spec);
    tk3_zeta_2spec_plot.set_log_x(true);
    tk3_zeta_2spec_plot.set_title("$\\langle \\zeta \\zeta \\rangle$ power spectrum");

    transport::derived_data::wavenumber_series_table<double> tk3_zeta_2spec_table("dquad.threepf-1.zeta-2spec.table", "zeta-2spec-table.txt");
    tk3_zeta_2spec_table.add_line(tk3_zeta_2spec);

    transport::derived_data::zeta_threepf_wavenumber_series<double> tk3_zeta_3equspec = transport::derived_data::zeta_threepf_wavenumber_series<double>(ztk3,
                                                                                                                                                        transport::derived_data::filter::time_filter(time_config_filter),
                                                                                                                                                        transport::derived_data::filter::threepf_kconfig_filter(all_equilateral));
    tk3_zeta_3equspec.set_klabel_meaning(transport::derived_data::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_zeta_3equspec_plot("dquad.threepf-1.zeta-3equspec", "zeta-3equspec.pdf");
    tk3_zeta_3equspec_plot.add_line(tk3_zeta_3equspec);
    tk3_zeta_3equspec_plot.set_log_x(true);
    tk3_zeta_3equspec_plot.set_title("$\\langle \\zeta \\zeta \\zeta \\rangle$ equilateral configuration");

    transport::derived_data::wavenumber_series_table<double> tk3_zeta_3equspec_table("dquad.threepf-1.zeta-3equspec.table", "zeta-3equspec-table.txt");
    tk3_zeta_3equspec_table.add_line(tk3_zeta_3equspec);

    transport::derived_data::zeta_reduced_bispectrum_wavenumber_series<double> tk3_zeta_redbsp_spec(ztk3,
                                                                                                    transport::derived_data::filter::time_filter(time_config_filter),
                                                                                                    transport::derived_data::filter::threepf_kconfig_filter(all_squeezed));
    tk3_zeta_redbsp_spec.set_klabel_meaning(transport::derived_data::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_redbsp_spec_plot("dquad.threepf-1.redbsp-spec", "redbsp-spec.pdf");
    tk3_redbsp_spec_plot.add_line(tk3_zeta_redbsp_spec);
    tk3_redbsp_spec_plot.set_log_x(true);
    tk3_redbsp_spec_plot.set_title("Reduced bispectrum in the squeezed limit");

    transport::derived_data::wavenumber_series_table<double> tk3_redbsp_spec_table("dquad.threepf-1.redbsp-spec-table", "redbsp-spec-table.txt");
    tk3_redbsp_spec_table.add_line(tk3_zeta_redbsp_spec);

    transport::fNL_task<double> ztk3_loc("dquad.threepf-1.fNL_local", ztk3, transport::derived_data::fNL_local_template);
    transport::fNL_task<double> ztk3_orth("dquad.threepf-1.fNL_orth", ztk3, transport::derived_data::fNL_ortho_template);
    transport::fNL_task<double> ztk3_equi("dquad.threepf-1.fNL_equi", ztk3, transport::derived_data::fNL_equi_template);

    transport::derived_data::fNL_time_series<double> fNLloc_time_series(ztk3_loc, transport::derived_data::filter::time_filter(timeseries_filter));

    transport::derived_data::fNL_time_series<double> fNLequi_time_series(ztk3_orth, transport::derived_data::filter::time_filter(timeseries_filter));

    transport::derived_data::fNL_time_series<double> fNLortho_time_series(ztk3_equi, transport::derived_data::filter::time_filter(timeseries_filter));

    transport::derived_data::time_series_plot<double> fNLloc_plot("dquad.threepf-1.fNLlocal", "fNLlocal.pdf");
    fNLloc_plot.add_line(fNLloc_time_series);
    fNLloc_plot.add_line(fNLequi_time_series);
    fNLloc_plot.add_line(fNLortho_time_series);
    fNLloc_plot.add_line(tk3_zeta_redbsp);
    fNLloc_plot.set_title(false);

    transport::derived_data::time_series_table<double> fNLloc_table("dquad.threepf-1.fNLlocal-table", "fNLlocal-table.txt");
    fNLloc_table.add_line(fNLloc_time_series);
    fNLloc_table.add_line(fNLequi_time_series);
    fNLloc_table.add_line(fNLortho_time_series);
    fNLloc_table.add_line(tk3_zeta_redbsp);

    // construct output tasks
    transport::output_task<double> twopf_output = transport::output_task<double>("dquad.twopf-1.output", tk2_bg_plot);
    twopf_output.add_element(tk2_twopf_real_plot);
    twopf_output.add_element(tk2_twopf_imag_plot);
    twopf_output.add_element(tk2_twopf_total_plot);

    transport::output_task<double> threepf_output = transport::output_task<double>("dquad.threepf-1.output", tk3_bg_plot);
    threepf_output.add_element(tk3_twopf_real_plot);
    threepf_output.add_element(tk3_twopf_imag_plot);
    threepf_output.add_element(tk3_twopf_total_plot);
    threepf_output.add_element(tk3_threepf_field_equi_plot);
    threepf_output.add_element(tk3_threepf_field_equi_table);
//		threepf_output.add_element(tk3_threepf_mma_equi_plot);
//		threepf_output.add_element(tk3_threepf_mma_equi_table);
    threepf_output.add_element(tk3_threepf_deriv_equi_plot);
    threepf_output.add_element(tk3_threepf_deriv_equi_table);
    threepf_output.add_element(tk3_threepf_field_sq_plot);
    threepf_output.add_element(tk3_threepf_field_sq_table);
//    threepf_output.add_element(tk3_threepf_mma_sq_plot);
//    threepf_output.add_element(tk3_threepf_mma_sq_table);
    threepf_output.add_element(tk3_threepf_deriv_sq_plot);
    threepf_output.add_element(tk3_threepf_deriv_sq_table);
//		threepf_output.add_element(tk3_mixed_plot);
//    threepf_output.add_element(tk3_check_shift);
//		threepf_output.add_element(tk3_check_shift_table);
    threepf_output.add_element(tk3_zeta_twopf);
    threepf_output.add_element(tk3_zeta_equi);
    threepf_output.add_element(tk3_zeta_equi_table);
    threepf_output.add_element(tk3_zeta_sq);
    threepf_output.add_element(tk3_zeta_sq_table);
    threepf_output.add_element(tk3_redbsp);
    threepf_output.add_element(tk3_redbsp_table);
    threepf_output.add_element(tk3_twopf_real_spec);
//    threepf_output.add_element(tk3_twopf_imag_spec);
//    threepf_output.add_element(tk3_twopf_total_spec);
//    threepf_output.add_element(tk3_twopf_total_tab);
    threepf_output.add_element(tk3_threepf_equi_spec);
    threepf_output.add_element(tk3_threepf_equi_spec_tab);
    threepf_output.add_element(tk3_zeta_2spec_plot);
    threepf_output.add_element(tk3_zeta_2spec_table);
    threepf_output.add_element(tk3_zeta_3equspec_plot);
    threepf_output.add_element(tk3_zeta_3equspec_table);
    threepf_output.add_element(tk3_redbsp_spec_plot);
    threepf_output.add_element(tk3_redbsp_spec_table);

    transport::output_task<double> fNL_output = transport::output_task<double>("dquad.threepf-1.fNLs", fNLloc_plot);
    fNL_output.add_element(fNLloc_table);

    // write output tasks to the database
    repo->commit_task(twopf_output);
    repo->commit_task(threepf_output);
    repo->commit_task(fNL_output);

    delete mgr;     // task_manager adopts its repository and destroys it silently; also destroys any registered models

    return (EXIT_SUCCESS);
  }
