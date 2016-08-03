# Overview

**CppTransport** is an automated platform that can be used to solve for the 2 and 3-point functions of the perturbations produced during an inflationary epoch in the very early universe. The present version is implemented for models with canonical kinetic terms, although the underlying method is quite general and can be scaled to handle models with a non-trivial field-space metric or an even more general non-canonical Lagrangian. Future versions of **CppTransport** may include support for these generalizations.

The numerical results produced by **CppTransport** are accurate to tree-level, including all quantum effects. In particular this includes

* effects generated by interference between growing and decaying modes near horizon exit
* effects generated by a nontrivial mass hierarchy, correctly accounting for modes which are heavy or light compared to the inflationary scale
* effects from trajectory bending and species correlation near horizon exit
* superhorizon evolution of the perturbations

The present version of **CppTransport** has been written by David Seery at the University of Sussex. It implements a formalism developed by Mafalda Dias, Jonathan Frazer, David Mulryne and David Seery.

A number of different tools implementing the same formalism, but with slightly different design goals, have been developed. These include a Python platform and a Mathematica platform. These tools are available from [transportmethod.com](http://transportmethod.com).

# Releases

The current release of CppTransport is 2016.2 (3 August 2016). This release can be identified via a DOI linking to a deposit at [zenodo.org](https://zenodo.org/record/59403). .tar.gz archives for each release are also available from [GitHub](https://github.com/ds283/CppTransport/releases).

* 2016.2 (3 August 2016) [Source code DOI:10.5281/zenodo.59403](http://dx.doi.org/10.5281/zenodo.59403) | [User guide DOI:10.5281/zenodo.59406](http://dx.doi.org/10.5281/zenodo.59406)
* 2016.1 (26 July 2016) [Source code DOI:10.5281/zenodo.58710](http://dx.doi.org/10.5281/zenodo.58710) | [User guide DOI:10.5281/zenodo.58714](http://dx.doi.org/10.5281/zenodo.58714)

# Licensing

**CppTransport** is distributed under the GNU General Public License version 2, or (at your option) any later version. This license is bundled with the source code as LICENSE.txt.

The **CppTransport** platform depends on, or incorporates, portions of other open source projects. For details, see the file NOTICE.txt.

# Examples and cookbook

A long worked example of double-quadratic inflation is included in the [user guide](http://dx.doi.org/10.5281/zenodo.58714).

Alternatively, the C++ codes and model description files required to reproduce the plots in Dias et al. (2016) are available in a [separate GitHub repository](https://github.com/ds283/transport-paper).

Some examples and cookbook-style snippets for common tasks are collected on the [Wiki](https://github.com/ds283/CppTransport/wiki).

# How to cite *CppTransport*

If you use **CppTransport** to produce numerical results supporting your research, then we would appreciate a citation to the main reference:

* *Numerical evaluation of the bispectrum in multiple-field inflation*, Mafalda Dias, Jonathan Frazer, David Mulryne and David Seery. arXiv:16xx.yyyyy DOI:

Specific documentation for **CppTransport** can be cited using

* *CppTransport*: a platform to automate calculation of inflationary correlation functions, David Seery. arXiv:16xx.yyyyy [DOI:10.5281/zenodo.58714](http://dx.doi.org/10.5281/zenodo.58714)

### Original literature

The use of evolution equations to compute n-point functions synthesized during an inflationary epoch (including all quantum effects) was described in

* *Transporting non-Gaussianity from sub to super-horizon scales*, David J. Mulryne. JCAP 1309 (2013) 010 [arXiv:1302.3842](http://arxiv.org/abs/arXiv:1302.3842) [DOI:10.1088/1475-7516/2013/09/010](http://dx.doi.org/10.1088/1475-7516/2013/09/010)

Concrete numerical details are described in the paper *Numerical evaluation of the bispectrum in multiple-field inflation* listed above, or were first given for the two-point function in

* *Computing observables in curved multifield models of inflation - A guide (with code) to the transport method*, Mafalda Dias, Jonathan Frazer and David Seery. JCAP 1512 (2015) 12 030 [arXiv:1502.03125](http://arxiv.org/abs/arXiv:1502.03125) [DOI:10.1088/1475-7516/2015/12/030](http://dx.doi.org/10.1088/1475-7516/2015/12/030)

A full list of references is given in the **CppTransport** documentation, or can be found at [transportmethod.com](https://transportmethod.com/method/).

# Acknowledgments

Development of the **CppTransport** platform has been supported by the grant *Precision tests of the inflationary scenario*, funded by the European Union’s Seventh Framework Programme (FP/2007–2013) and ERC Grant Agreement No. 308082.

Some development has been supported by other sources, including:

* The UK Science and Technology Facilities Council via grants ST/I000976/1 and ST/L000652/1, which funded the science programme at the University of Sussex Astronomy Centre from April 2011-March 2014 and April 2014-March 2017, respectively.

* The Leverhulme Trust via a Philip Leverhulme Prize.

* The National Science Foundation Grant No. PHYS-1066293 and the hospitality of the Aspen Center for Physics.

* The hospitality of the Higgs Centre for Theoretical Physics at the University of Edinburgh, and the Centre for Astronomy & Particle Physics at the University of Nottingham.
