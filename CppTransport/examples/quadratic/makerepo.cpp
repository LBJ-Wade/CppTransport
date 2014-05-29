//
// Created by David Seery on 29/05/2014.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "quad_basic.h"

#include "transport-runtime-api/manager/repository_creation_key.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

const double m_phi	  = 1E-5 * M_Planck;

const double phi_init = 16;


// ****************************************************************************


// filter to determine which time values are included on plots - we just use them all
bool time_filter(const transport::derived_data::filter::time_filter_data&)
	{
    return(true); // plot all values of the time
	}


// filter to determine which 2pf kconfig values are included on plots.
// to cut down the sample, here I use only the largest
bool twopf_kconfig_filter(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
	{
    return(data.max); // plot only the largest k
	}


// filter to determine which 3pf kconfig values are included on plots.
// to cut down the sample, here I use only the largest
bool threepf_kconfig_filter(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
	{
    return(data.kt_max); // plot only the largest k_t
	}


// filter for near-equilateral 3pf k configurations - pick the largest and smallest kt
bool threepf_kconfig_equilateral(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
	{
    return(fabs(data.alpha) < 0.01 && fabs(data.beta-(1.0/3.0)) < 0.01 && (data.kt_max || data.kt_min));
	}


// filter for near-squeezed 3pf k-configurations
bool threepf_kconfig_near_squeezed(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
	{
    return(fabs(data.beta) > 0.85);
	}


// filter for most-squeezed 3pf k-configuration
bool threepf_kconfig_most_squeezed(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
	{
    return(data.beta_max);  // plot only the largest beta
	}


// filter for most-equilateral 3pf k-configuration
bool threepf_kconfig_most_equilateral(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
	{
    return(fabs(data.alpha) < 0.01 && fabs(data.beta-(1.0/3.0)) < 0.01 && data.kt_max);
	}


int main(int argc, char* argv[])
	{
    if(argc != 2)
	    {
        std::cerr << "makerepo: Too few arguments. Expected repository name" << std::endl;
        exit(EXIT_FAILURE);
	    }

    transport::repository_creation_key key;

    transport::repository<double>* repo = transport::repository_factory<double>(argv[1], key);

    // set up an instance of a manager
    transport::task_manager<double>* mgr = new transport::task_manager<double>(0, nullptr, repo);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    transport::quadratic_basic<double>* model = new transport::quadratic_basic<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params = { m_phi };
    transport::parameters<double> params      =
	                                  transport::parameters<double>(M_Planck, init_params, model->get_param_names(),
	                                                                model->params_validator_factory());

    const std::vector<double> init_values = { phi_init };

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 7.0;  // horizon-crossing occurs at 7 e-folds from init_values
    const double Npre   = 7.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 55.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions
    transport::initial_conditions<double> ics =
	                                          transport::initial_conditions<double>("quadratic-1", params, init_values, model->get_state_names(),
	                                                                                Ninit, Ncross, Npre,
	                                                                                model->ics_validator_factory(),
	                                                                                model->ics_finder_factory());

    const unsigned int t_samples = 5000;       // record 5000 samples - enough to find a good stepsize

    transport::range<double> times = transport::range<double >(Ninit, Nmax+Npre, t_samples);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kmin      = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double        kmax      = exp(3.0);   // end with the mode which exits the horizon at N=N*+3
    const unsigned int  k_samples = 4 ;         // number of k-points

    transport::range<double> ks = transport::range<double>(kmin, kmax, k_samples, transport::range<double>::logarithmic);

    std::cout << ks;

    // construct a threepf task
    transport::threepf_task<double> tk3 = transport::threepf_task<double>("quadratic.threepf-1", ics, times, ks, model->kconfig_kstar_factory());

    std::cout << tk3;

    // construct a twopf task
    transport::twopf_task<double> tk2 = transport::twopf_task<double>("quadratic.twopf-1", ics, times, ks, model->kconfig_kstar_factory());

    // write each initial conditions/parameter specification and integration specification into the model repository
    repo->write_task(tk2, model);
    repo->write_task(tk3, model);

    // construct some derived data products; first, simply plots of the background

    transport::index_selector<1> bg_sel(model->get_N_fields());
    bg_sel.all();

    transport::derived_data::background_time_series<double> tk2_bg = transport::derived_data::background_time_series<double>(tk2, model, bg_sel, transport::derived_data::filter::time_filter(time_filter));

    transport::derived_data::background_time_series<double> tk3_bg = transport::derived_data::background_time_series<double>(tk3, model, bg_sel, transport::derived_data::filter::time_filter(time_filter));

    transport::derived_data::time_series_plot<double> tk2_bg_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.twopf-1.background", "background.pdf");
    tk2_bg_plot.add_line(tk2_bg);
    tk2_bg_plot.set_title_text("Background fields");

    transport::derived_data::time_series_plot<double> tk3_bg_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.background", "background.pdf");
    tk3_bg_plot.add_line(tk3_bg);
    tk3_bg_plot.set_title_text("Background fields");

    // plots of some components of the twopf

    transport::index_selector<2> twopf_fields(model->get_N_fields());
    transport::index_selector<2> twopf_cross(model->get_N_fields());
    twopf_fields.none();
    twopf_cross.none();

    // field-field correlations
    std::array<unsigned int, 2> index_set_a = { 0, 0 };

    // momenta-field correlations; the imaginary 2pf only has these cross-terms
    std::array<unsigned int, 2> index_set_d = { 1, 0 };

    twopf_fields.set_on(index_set_a);

    twopf_cross.set_on(index_set_d);

    transport::derived_data::twopf_time_series<double> tk2_twopf_real_group =
	                                                       transport::derived_data::twopf_time_series<double>(tk2, model, twopf_fields,
	                                                                                                          transport::derived_data::filter::time_filter(time_filter),
	                                                                                                          transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk2_twopf_real_group.set_klabel_meaning(transport::derived_data::derived_line<double>::conventional);

    transport::derived_data::twopf_time_series<double> tk2_twopf_imag_group =
	                                                       transport::derived_data::twopf_time_series<double>(tk2, model, twopf_cross,
	                                                                                                          transport::derived_data::filter::time_filter(time_filter),
	                                                                                                          transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk2_twopf_imag_group.set_klabel_meaning(transport::derived_data::derived_line<double>::conventional);
    tk2_twopf_imag_group.set_type(transport::derived_data::twopf_time_series<double>::imaginary);

    transport::derived_data::twopf_time_series<double> tk3_twopf_real_group =
	                                                       transport::derived_data::twopf_time_series<double>(tk3, model, twopf_fields,
	                                                                                                          transport::derived_data::filter::time_filter(time_filter),
	                                                                                                          transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk3_twopf_real_group.set_klabel_meaning(transport::derived_data::derived_line<double>::conventional);

    transport::derived_data::twopf_time_series<double> tk3_twopf_imag_group =
	                                                       transport::derived_data::twopf_time_series<double>(tk3, model, twopf_cross,
	                                                                                                          transport::derived_data::filter::time_filter(time_filter),
	                                                                                                          transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk3_twopf_imag_group.set_klabel_meaning(transport::derived_data::derived_line<double>::conventional);
    tk3_twopf_imag_group.set_type(transport::derived_data::twopf_time_series<double>::imaginary);

    transport::derived_data::time_series_plot<double> tk2_twopf_real_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.twopf-1.twopf-real", "twopf-real.pdf");
    tk2_twopf_real_plot.add_line(tk2_twopf_real_group);
    tk2_twopf_real_plot.set_title_text("Real two-point function");
    tk2_twopf_real_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_plot<double> tk2_twopf_imag_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.twopf-1.twopf-imag", "twopf-imag.pdf");
    tk2_twopf_imag_plot.add_line(tk2_twopf_imag_group);
    tk2_twopf_imag_plot.set_title_text("Imaginary two-point function");
    tk2_twopf_imag_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_plot<double> tk2_twopf_total_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.twopf-1.twopf-total", "twopf-total.pdf");
    tk2_twopf_total_plot.add_line(tk2_twopf_real_group);
    tk2_twopf_total_plot.add_line(tk2_twopf_imag_group);
    tk2_twopf_total_plot.set_title_text("Two-point function");
    tk2_twopf_total_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_real_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.twopf-real", "twopf-real.pdf");
    tk3_twopf_real_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_real_plot.set_title_text("Real two-point function");
    tk3_twopf_real_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_imag_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.twopf-imag", "twopf-imag.pdf");
    tk3_twopf_imag_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_imag_plot.set_title_text("Imaginary two-point function");
    tk3_twopf_imag_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_total_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.twopf-total", "twopf-total.pdf");
    tk3_twopf_total_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_total_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_total_plot.set_title_text("Two-point function");
    tk3_twopf_total_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    // plots of some components of the threepf
    transport::index_selector<3> threepf_fields(model->get_N_fields());
    transport::index_selector<3> threepf_momenta(model->get_N_fields());
    threepf_fields.none();
    threepf_momenta.none();

    std::array<unsigned int, 3> sq_set_a    = { 0, 0, 0 };
    threepf_fields.set_on(sq_set_a);

    std::array<unsigned int, 3> sq_set_g    = { 1, 1, 1 };
    threepf_momenta.set_on(sq_set_g);

    // THREEPF FIELDS
    transport::derived_data::threepf_time_series<double> tk3_threepf_fields_equi =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_fields,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_equilateral));
    tk3_threepf_fields_equi.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_fields_equi.set_dot_meaning(transport::derived_data::derived_line<double>::derivatives);

    transport::derived_data::time_series_plot<double> tk3_threepf_field_equi_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.f-equi", "f-equi.pdf");
    tk3_threepf_field_equi_plot.add_line(tk3_threepf_fields_equi);
    tk3_threepf_field_equi_plot.set_title_text("Three-point function");
    tk3_threepf_field_equi_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_field_equi_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.f-equi.table", "f-equi-table.txt");
    tk3_threepf_field_equi_table.add_line(tk3_threepf_fields_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_fields_sq =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_fields,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_squeezed));
    tk3_threepf_fields_sq.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_fields_sq.set_dot_meaning(transport::derived_data::derived_line<double>::derivatives);
    tk3_threepf_fields_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_field_sq_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.f-sq", "f-sq.pdf");
    tk3_threepf_field_sq_plot.add_line(tk3_threepf_fields_sq);
    tk3_threepf_field_sq_plot.set_title_text("Three-point function");
    tk3_threepf_field_sq_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_field_sq_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.f-sq.table", "f-sq-table.txt");
    tk3_threepf_field_sq_table.add_line(tk3_threepf_fields_sq);


    // THREEPF DERIVATIVES
    transport::derived_data::threepf_time_series<double> tk3_threepf_deriv_equi =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_momenta,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_equilateral));
    tk3_threepf_deriv_equi.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_deriv_equi.set_dot_meaning(transport::derived_data::derived_line<double>::derivatives);

    transport::derived_data::time_series_plot<double> tk3_threepf_deriv_equi_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.d-equi", "d-equi.pdf");
    tk3_threepf_deriv_equi_plot.add_line(tk3_threepf_deriv_equi);
    tk3_threepf_deriv_equi_plot.set_title_text("Three-point function");
    tk3_threepf_deriv_equi_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_deriv_equi_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.d-equi.table", "d-equi-table.txt");
    tk3_threepf_deriv_equi_table.add_line(tk3_threepf_deriv_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_deriv_sq =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_momenta,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_squeezed));
    tk3_threepf_deriv_sq.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_deriv_sq.set_dot_meaning(transport::derived_data::derived_line<double>::derivatives);
    tk3_threepf_deriv_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_deriv_sq_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.d-sq", "d-sq.pdf");
    tk3_threepf_deriv_sq_plot.add_line(tk3_threepf_deriv_sq);
    tk3_threepf_deriv_sq_plot.set_title_text("Three-point function");
    tk3_threepf_deriv_sq_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_deriv_sq_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.d-sq.table", "d-sq-table.txt");
    tk3_threepf_deriv_sq_table.add_line(tk3_threepf_deriv_sq);

    // THREEPF MOMENTA
    transport::derived_data::threepf_time_series<double> tk3_threepf_mma_equi =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_momenta,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_equilateral));
    tk3_threepf_mma_equi.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_mma_equi.set_dot_meaning(transport::derived_data::derived_line<double>::momenta);

    transport::derived_data::time_series_plot<double> tk3_threepf_mma_equi_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.m-equi", "m-equi.pdf");
    tk3_threepf_mma_equi_plot.add_line(tk3_threepf_mma_equi);
    tk3_threepf_mma_equi_plot.set_title_text("Three-point function");
    tk3_threepf_mma_equi_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_mma_equi_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.m-equi.table", "m-equi-table.txt");
    tk3_threepf_mma_equi_table.add_line(tk3_threepf_mma_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_mma_sq =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_momenta,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_most_squeezed));
    tk3_threepf_mma_sq.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_mma_sq.set_dot_meaning(transport::derived_data::derived_line<double>::momenta);
    tk3_threepf_mma_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_mma_sq_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.m-sq", "m-sq.pdf");
    tk3_threepf_mma_sq_plot.add_line(tk3_threepf_mma_sq);
    tk3_threepf_mma_sq_plot.set_title_text("Three-point function");
    tk3_threepf_mma_sq_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_mma_sq_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.m-sq.table", "m-sq-table.txt");
    tk3_threepf_mma_sq_table.add_line(tk3_threepf_mma_sq);



    transport::derived_data::time_series_plot<double> tk3_mixed_plot =
	                                                      transport::derived_data::time_series_plot<double>("quadratic.threepf-1.mixed", "mixed.pdf");

    tk3_mixed_plot.add_line(tk3_threepf_fields_equi);
    tk3_mixed_plot.add_line(tk3_twopf_real_group);
    tk3_mixed_plot.set_title_text("Two- and three-point functions");
    tk3_mixed_plot.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    // pick out the shift between derivative and momenta 3pfs
    transport::index_selector<3> threepf_mmta(model->get_N_fields());
    threepf_mmta.none();
    std::array<unsigned int, 3> sq_mmta_a = { 1, 0, 0 };
    threepf_mmta.set_on(sq_mmta_a);

    transport::derived_data::threepf_time_series<double> tk3_threepf_derivs =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_mmta,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_filter));
    tk3_threepf_derivs.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_derivs.set_dot_meaning(transport::derived_data::derived_line<double>::derivatives);
    tk3_threepf_derivs.set_use_alpha_label(true);
    tk3_threepf_derivs.set_use_beta_label(true);

    transport::derived_data::threepf_time_series<double> tk3_threepf_momenta =
	                                                         transport::derived_data::threepf_time_series<double>(tk3, model, threepf_mmta,
	                                                                                                              transport::derived_data::filter::time_filter(time_filter),
	                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_filter));
    tk3_threepf_momenta.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_threepf_momenta.set_dot_meaning(transport::derived_data::derived_line<double>::momenta);
    tk3_threepf_momenta.set_use_alpha_label(true);
    tk3_threepf_momenta.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_check_shift = transport::derived_data::time_series_plot<double>("quadratic.threepf-1.checkshift", "checkshift.pdf");

    tk3_check_shift.add_line(tk3_threepf_derivs);
    tk3_check_shift.add_line(tk3_threepf_momenta);
    tk3_check_shift.set_title_text("Comparison of derivative and momenta 3pf");
    tk3_check_shift.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    transport::derived_data::time_series_table<double> tk3_check_shift_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.checkshift.table", "checkshift-table.txt");

    tk3_check_shift_table.add_line(tk3_threepf_derivs);
    tk3_check_shift_table.add_line(tk3_threepf_momenta);

    // check the zeta twopf
    transport::derived_data::zeta_twopf_time_series<double> tk3_zeta_twopf_group = transport::derived_data::zeta_twopf_time_series<double>(tk3, model,
                                                                                                                                           transport::derived_data::filter::time_filter(time_filter),
                                                                                                                                           transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));

    transport::derived_data::time_series_plot<double> tk3_zeta_twopf = transport::derived_data::time_series_plot<double>("quadratic.threepf-1.zeta-twopf", "zeta-twopf.pdf");

    tk3_zeta_twopf.add_line(tk3_zeta_twopf_group);
    tk3_zeta_twopf.add_line(tk3_twopf_real_group);
    tk3_zeta_twopf.set_title_text("Comparison of $\\zeta$ and field 2pfs");
    tk3_zeta_twopf.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    // check the zeta threepf
    transport::derived_data::zeta_threepf_time_series<double> tk3_zeta_equi_group = transport::derived_data::zeta_threepf_time_series<double>(tk3, model,
                                                                                                                                              transport::derived_data::filter::time_filter(time_filter),
                                                                                                                                              transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_equilateral));
    tk3_zeta_equi_group.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_zeta_equi_group.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_equi = transport::derived_data::time_series_plot<double>("quadratic.threepf-1.zeta-equi", "zeta-equi.pdf");
    tk3_zeta_equi.add_line(tk3_zeta_equi_group);
    tk3_zeta_equi.set_title_text("3pf of $\\zeta$ near equilateral configurations");

    // set up a table too
    transport::derived_data::time_series_table<double> tk3_zeta_equi_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.zeta-equi.table", "zeta-equi-table.txt");
    tk3_zeta_equi_table.add_line(tk3_zeta_equi_group);

    transport::derived_data::zeta_threepf_time_series<double> tk3_zeta_sq_group = transport::derived_data::zeta_threepf_time_series<double>(tk3, model,
                                                                                                                                            transport::derived_data::filter::time_filter(time_filter),
                                                                                                                                            transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_near_squeezed));
		tk3_zeta_sq_group.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_zeta_sq_group.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_sq = transport::derived_data::time_series_plot<double>("quadratic.threepf-1.zeta-sq", "zeta-sq.pdf");
    tk3_zeta_sq.add_line(tk3_zeta_sq_group);
    tk3_zeta_sq.set_title_text("3pf of $\\zeta$ near squeezed configurations");

    // set up a table too
    transport::derived_data::time_series_table<double> tk3_zeta_sq_table = transport::derived_data::time_series_table<double>("quadratic.threepf-1.zeta-sq.table", "zeta-sq-table.txt");
    tk3_zeta_sq_table.add_line(tk3_zeta_sq_group);

    std::cout << "3pf equilateral plot:" << std::endl << tk3_zeta_equi << std::endl;

    std::cout << "3pf squeezed plot:" << std::endl<< tk3_zeta_sq << std::endl;

    // write derived data products representing these background plots to the database
    repo->write_derived_product(tk2_bg_plot);
    repo->write_derived_product(tk2_twopf_real_plot);
    repo->write_derived_product(tk2_twopf_imag_plot);
    repo->write_derived_product(tk2_twopf_total_plot);

    repo->write_derived_product(tk3_bg_plot);
    repo->write_derived_product(tk3_twopf_real_plot);
    repo->write_derived_product(tk3_twopf_imag_plot);
    repo->write_derived_product(tk3_twopf_total_plot);
    repo->write_derived_product(tk3_threepf_field_equi_plot);
    repo->write_derived_product(tk3_threepf_deriv_equi_plot);
    repo->write_derived_product(tk3_threepf_mma_equi_plot);
    repo->write_derived_product(tk3_threepf_field_sq_plot);
    repo->write_derived_product(tk3_threepf_deriv_sq_plot);
    repo->write_derived_product(tk3_threepf_mma_sq_plot);
    repo->write_derived_product(tk3_mixed_plot);

    repo->write_derived_product(tk3_check_shift);

    repo->write_derived_product(tk3_zeta_twopf);
    repo->write_derived_product(tk3_zeta_equi);
    repo->write_derived_product(tk3_zeta_sq);

    repo->write_derived_product(tk3_threepf_field_equi_table);
    repo->write_derived_product(tk3_threepf_deriv_equi_table);
    repo->write_derived_product(tk3_threepf_mma_equi_table);
    repo->write_derived_product(tk3_threepf_field_sq_table);
    repo->write_derived_product(tk3_threepf_deriv_sq_table);
    repo->write_derived_product(tk3_threepf_mma_sq_table);
    repo->write_derived_product(tk3_check_shift_table);
    repo->write_derived_product(tk3_zeta_equi_table);
    repo->write_derived_product(tk3_zeta_sq_table);

    // construct output tasks
    transport::output_task<double> twopf_output   = transport::output_task<double>("quadratic.twopf-1.output", tk2_bg_plot);
    twopf_output.add_element(tk2_twopf_real_plot);
    twopf_output.add_element(tk2_twopf_imag_plot);
    twopf_output.add_element(tk2_twopf_total_plot);

    transport::output_task<double> threepf_output = transport::output_task<double>("quadratic.threepf-1.output", tk3_bg_plot);
    threepf_output.add_element(tk3_twopf_real_plot);
    threepf_output.add_element(tk3_twopf_imag_plot);
    threepf_output.add_element(tk3_twopf_total_plot);
    threepf_output.add_element(tk3_threepf_field_equi_plot);
    threepf_output.add_element(tk3_threepf_field_equi_table);
    threepf_output.add_element(tk3_threepf_mma_equi_plot);
    threepf_output.add_element(tk3_threepf_mma_equi_table);
    threepf_output.add_element(tk3_threepf_deriv_equi_plot);
    threepf_output.add_element(tk3_threepf_deriv_equi_table);
    threepf_output.add_element(tk3_threepf_field_sq_plot);
    threepf_output.add_element(tk3_threepf_field_sq_table);
    threepf_output.add_element(tk3_threepf_mma_sq_plot);
    threepf_output.add_element(tk3_threepf_mma_sq_table);
    threepf_output.add_element(tk3_threepf_deriv_sq_plot);
    threepf_output.add_element(tk3_threepf_deriv_sq_table);
    threepf_output.add_element(tk3_mixed_plot);
    threepf_output.add_element(tk3_check_shift);
    threepf_output.add_element(tk3_check_shift_table);
    threepf_output.add_element(tk3_zeta_twopf);
    threepf_output.add_element(tk3_zeta_equi);
    threepf_output.add_element(tk3_zeta_equi_table);
    threepf_output.add_element(tk3_zeta_sq);
    threepf_output.add_element(tk3_zeta_sq_table);

    std::cout << "quadratic.threepf-1 output task:" << std::endl << threepf_output << std::endl;

    // write output tasks to the database
    repo->write_task(twopf_output);
    repo->write_task(threepf_output);

    std::string package_json = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_package_document(ics.get_name());
    std::cout << "Package JSON document:" << std::endl << package_json << std::endl << std::endl;

    std::string task2_json = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_task_document(tk2.get_name());
    std::cout << "2pf integration JSON document:" << std::endl << task2_json << std::endl << std::endl;

    std::string task3_json = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_task_document(tk3.get_name());
    std::cout << "3pf integration JSON document:" << std::endl << task3_json << std::endl << std::endl;

    std::string out3_json  = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_task_document(threepf_output.get_name());
    std::cout << "3pf output task document:" << std::endl << out3_json << std::endl << std::endl;

    delete mgr;     // task_manager adopts its repository and destroys it silently; also destroys any registered models

    return(EXIT_SUCCESS);
	}