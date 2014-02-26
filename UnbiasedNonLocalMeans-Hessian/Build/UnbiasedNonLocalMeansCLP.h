// This file was automatically generated by:
//  ./GenerateCLP --logoFiles /home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/ModuleLogo.h --InputXML /home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/UnbiasedNonLocalMeans.xml --OutputCxx /home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/UnbiasedNonLocalMeansCLP.h
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>

#include <itksys/ios/sstream>

#include "tclap/CmdLine.h"
#include "ModuleProcessInformation.h"

#ifdef WIN32
#define Module_EXPORT __declspec(dllexport)
#else
#define Module_EXPORT 
#endif

#if defined(main) && !defined(REGISTER_TEST)
// If main defined as a preprocessor symbol, redefine it to the expected entry point.
#undef main
#define main ModuleEntryPoint

extern "C" {
  Module_EXPORT char *GetXMLModuleDescription();
  Module_EXPORT int ModuleEntryPoint(int, char*[]);
  Module_EXPORT unsigned char *GetModuleLogo(int *width, int *height, int *pixel_size, unsigned long *bufferLength);
}
#endif

extern "C" {
Module_EXPORT char XMLModuleDescription[] = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<executable>\n"
"  <category>Filtering.Denoising</category>\n"
"  <title>Unbiased NLM for MRI</title>\n"
"  <description>\n"
"  This module implements a fast version of the popular Non-Local Means filter for image denoising. This algorithm filters each pixel as a weighted average of its neighbors in a large vicinity. The weights are computed based on the similarity of each neighbor with the voxel to be denoised.\n In the original formulation a patch with a certain radius is centered in each of the voxels, and the Mean Squared Error between each pair of corresponding voxels is computed. In this implementation, only the mean value and gradient components are compared. This, together with an efficient memory management, can attain a speed-up of nearly 20x. Besides, the filtering is more accurate than the original with poor SNR.\n This code is intended for its use with MRI (or any other Rician-distributed modality): the second order moment is estimated, then we subtract twice the squared power of noise, and finally we take the square root of the result to remove the Rician bias.\n The original implementation of the NLM filter may be found in:\n A. Buades, B. Coll, J. Morel, \"A review of image denoising algorithms, with a new one\", Multiscale Modelling and Simulation 4(2): 490-530. 2005.\n The correction of the Rician bias is described in the following reference (among others):\n S. Aja-Fernandez, K. Krissian, \"An unbiased Non-Local Means scheme for DWI filtering\", in: Proceedings of the MICCAI Workshop on Computational Diffusion MRI, 2008, pp. 277-284.\n The whole description of this version may be found in the following paper (please, cite it if you are willing to use this software):\n A. Tristan-Vega, V. Garcia Perez, S. Aja-Fenandez, and C.-F. Westin, \"Efficient and Robust Nonlocal Means Denoising of MR Data Based on Salient Features Matching\", Computer Methods and Programs in Biomedicine. (Accepted for publication) 2011.\n"
"  </description>\n"
"  <version>0.0.1.$Revision: 1 $(beta)</version>\n"
"  <documentation-url>http://www.slicer.org/slicerWiki/index.php/Modules:UnbiasedNonLocalMeans-Documentation-3.6</documentation-url>\n"
"  <license></license>\n"
"  <contributor>Antonio Tristan Vega, Veronica Garcia-Perez, Santiago Aja-Fernandez, Carl-Fredrik Westin\n"
"  </contributor>\n"
"  <acknowledgements>Supported by grant number FMECD-2010/71131616E from the Spanish Ministry of Education/Fulbright Committee\n"
"  </acknowledgements>\n"
"  <parameters>\n"
"  \n"
"    <label>Unbiased NLM for MRI</label>\n"
"    <description>Parameters for UNLM</description>\n"
"    \n"
"    <float>\n"
"      <name>iSigma</name>\n"
"      <label>Noise power</label>\n"
"      <longflag>--sigma</longflag>\n"
"      <description>The root power of noise (sigma) in the complex Gaussian process the Rician comes from. If it is underestimated, the algorithm fails to remove the noise. If it is overestimated, over-blurring is likely to occur.</description>\n"
"      <default>5.0</default>\n"
"    </float>\n"
"    \n"
"    <integer-vector>\n"
"      <name>iRadiusSearch</name>\n"
"      <label>Search radius</label>\n"
"      <longflag>--rs</longflag>\n"
"      <description>The algorithm search for similar voxels in a neighborhood of this radius (radii larger than 5,5,5 are very slow, and the results can be only marginally better. Small radii may fail to effectively remove the noise).</description>\n"
"      <default>3,3,3</default>\n"
"    </integer-vector>\n"
"\n"
"    <integer-vector>\n"
"      <name>iRadiusComp</name>\n"
"      <label>Comparison radius</label>\n"
"      <longflag>--rc</longflag>\n"
"      <description>Similarity between blocks is computed as the difference between mean values and gradients. These parameters are computed fitting a hyperplane with LS inside a neighborhood of this size</description>\n"
"      <default>1,1,1</default>\n"
"    </integer-vector>\n"
"\n"
"    <float>\n"
"      <name>iH</name>\n"
"      <label>h parameter</label>\n"
"      <longflag>--hp</longflag>\n"
"      <description>This parameter is related to noise; the larger the parameter, the more aggressive the filtering. Should be near 1, and only values between 0.8 and 1.2 are allowed</description>\n"
"      <default>1.0</default>\n"
"    </float>\n"
"    \n"
"    <float>\n"
"      <name>iPs</name>\n"
"      <label>Preselection threshold</label>\n"
"      <longflag>--ps</longflag>\n"
"      <description>To accelerate computations, preselection is used: if the normalized difference is above this threshold, the voxel will be discarded (non used for average)</description>\n"
"      <default>2.0</default>\n"
"    </float>\n"
"\n"
"    <double>\n"
"	<name>iMinScale</name>\n"
"      <label>Minimum scale level</label>\n"
"      <longflag>--min</longflag>\n"
"      <description>Minimum scale level</description>\n"
"      <default>1.0</default>\n"
"    </double>\n"
"    <double>\n"
"	<name>iMaxScale</name>\n"
"      <label>Maximum scale level</label>\n"
"      <longflag>--max</longflag>\n"
"      <description>Maximum scale level</description>\n"
"      <default>4.0</default>\n"
"    </double>\n"
"    <integer>\n"
"	<name>iNScales</name>\n"
"      <label>Number of scales</label>\n"
"      <longflag>--nos</longflag>\n"
"      <description>Number of scales</description>\n"
"      <default>5</default>\n"
"    </integer>\n"
"    <integer>\n"
"	<name>iOrder</name>\n"
"      <label>Order of derivatives</label>\n"
"      <longflag>--ord</longflag>\n"
"      <description></description>\n"
"      <default>2</default>\n"
"    </integer>\n"
"    <double>\n"
"	<name>iAlpha</name>\n"
"      <label>Parameter to control the delta strength</label>\n"
"      <longflag>--alpha</longflag>\n"
"      <description></description>\n"
"      <default>1.0</default>\n"
"    </double>\n"
"    <boolean>\n"
"	<name>iEstimatedDistanceMean</name>\n"
"      <label>Estimated Distance Mean</label>\n"
"      <longflag>--estDist</longflag>\n"
"      <description>Use Estimated Distance Mean</description>\n"
"      <default>false</default>\n"
"    </boolean>\n"
"    <boolean>\n"
"	<name>iNormalizedByFeatureStrength</name>\n"
"      <label>Normalized By Feature Strength</label>\n"
"      <longflag>--normByFeat</longflag>\n"
"      <description>Use Normalized By Feature Strength</description>\n"
"      <default>false</default>\n"
"    </boolean>\n"
"    <boolean>\n"
"	<name>iDeltaFeatureStrength</name>\n"
"      <label>Delta Feature Strength</label>\n"
"      <longflag>--deltaStr</longflag>\n"
"      <description>Use Delta Feature Strength</description>\n"
"      <default>false</default>\n"
"    </boolean>\n"
"  </parameters>\n"
"  \n"
"  <parameters>\n"
"  \n"
"    <label>IO</label>\n"
"    <description>Input/output parameters</description>\n"
"    <image type=\"scalar\">\n"
"      <name>inputVolume</name>\n"
"      <label>Input Volume</label>\n"
"      <channel>input</channel>\n"
"      <longflag>i</longflag>\n"
"      <description>Input MRI volume.</description>\n"
"    </image>\n"
"    \n"
"    <image type=\"scalar\">\n"
"      <name>outputVolume</name>\n"
"      <label>Output Volume</label>\n"
"      <channel>output</channel>\n"
"      <longflag>o</longflag>\n"
"      <description>Output (filtered) MRI volume.</description>\n"
"    </image>\n"
"\n"
"   <image type=\"scalar\">\n"
"      <name>featuresVolume</name>\n"
"      <label>Features Volume</label>\n"
"      <channel>output</channel>\n"
"      <longflag>feat</longflag>\n"
"      <description>Features volume.</description>\n"
"    </image>\n"
"\n"
"    <image type=\"scalar\">\n"
"      <name>scalesImageVolume</name>\n"
"      <label>Scales Volume</label>\n"
"      <channel>output</channel>\n"
"      <longflag>scale</longflag>\n"
"      <description>Scales volume.</description>\n"
"      <default>None</default>\n"
"    </image>\n"
"\n"
"    <image type=\"scalar\">\n"
"      <name>strengthImageVolume</name>\n"
"      <label>Strength Volume</label>\n"
"      <channel>output</channel>\n"
"      <longflag>strength</longflag>\n"
"      <description>Strength volume.</description>\n"
"      <default>None</default>\n"
"    </image>\n"
"\n"
"  </parameters>\n"
"\n"
"</executable>\n"
"\n"
;

#define static Module_EXPORT
#define const
#define image_ModuleLogo_width ModuleLogoWidth
#define image_ModuleLogo_height ModuleLogoHeight
#define image_ModuleLogo_pixel_size ModuleLogoPixelSize
#define image_ModuleLogo_length ModuleLogoLength
#define image_ModuleLogo ModuleLogoImage
#include "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/ModuleLogo.h"
#undef static
#undef const
#undef image_ModuleLogo_width
#undef image_ModuleLogo_height
#undef image_ModuleLogo_pixel_size
#undef image_ModuleLogo_length
#undef image_ModuleLogo
}

void
splitString (const std::string &text,
             const std::string &separators,
             std::vector<std::string> &words)
{
  const std::string::size_type n = text.length();
  std::string::size_type start = text.find_first_not_of(separators);
  while (start < n)
    {
    std::string::size_type stop = text.find_first_of(separators, start);
    if (stop > n) stop = n;
    words.push_back(text.substr(start, stop - start));
    start = text.find_first_not_of(separators, stop+1);
    }
}

void
splitFilenames (const std::string &text,
                std::vector<std::string> &words)
{
  const std::string::size_type n = text.length();
  bool quoted;
  std::string comma(",");
  std::string quote("\"");
  std::string::size_type start = text.find_first_not_of(comma);
  while (start < n)
    {
    quoted = false;
    std::string::size_type startq = text.find_first_of(quote, start);
    std::string::size_type stopq = text.find_first_of(quote, startq+1);
    std::string::size_type stop = text.find_first_of(comma, start);
    if (stop > n) stop = n;
    if (startq != std::string::npos && stopq != std::string::npos)
      {
      while (startq < stop && stop < stopq && stop != n)
         {
         quoted = true;
         stop = text.find_first_of(comma, stop+1);
         if (stop > n) stop = n;
         }
      }
    if (!quoted)
      {
      words.push_back(text.substr(start, stop - start));
      }
    else
      {
      words.push_back(text.substr(start+1, stop - start-2));
      }
    start = text.find_first_not_of(comma, stop+1);
    }
}

unsigned char *GetModuleLogo(int *width,
                             int *height,
                             int *pixel_size,
                             unsigned long *length)
{
  *width = ModuleLogoWidth;
  *height = ModuleLogoHeight;
  *pixel_size = ModuleLogoPixelSize;
  *length = ModuleLogoLength;
  return const_cast<unsigned char *>(ModuleLogoImage);
}

char *GetXMLModuleDescription()
{
   return XMLModuleDescription;
}

#define GENERATE_LOGO \
  if (argc >= 2 && (strcmp(argv[1],"--logo") == 0)) \
    { \
    int width, height, pixel_size;     \
    unsigned long length;  \
    unsigned char *logo = GetModuleLogo(&width, &height, &pixel_size, &length);  \
    std::cout << "LOGO" << std::endl;  \
    std::cout << width << std::endl;  \
    std::cout << height << std::endl;  \
    std::cout << pixel_size << std::endl;  \
    std::cout << length << std::endl;  \
    std::cout << logo << std::endl;  \
    return EXIT_SUCCESS;  \
    }
#define GENERATE_XML \
  if (argc >= 2 && (strcmp(argv[1],"--xml") == 0)) \
    { \
    std::cout << GetXMLModuleDescription(); \
    return EXIT_SUCCESS; \
    }
#define GENERATE_TCLAP \
    float iSigma = 5.0; \
    std::string iRadiusSearchTemp = "3,3,3"; \
    std::vector<int> iRadiusSearch; \
    std::string iRadiusCompTemp = "1,1,1"; \
    std::vector<int> iRadiusComp; \
    float iH = 1.0; \
    float iPs = 2.0; \
    double iMinScale = 1.0; \
    double iMaxScale = 4.0; \
    int iNScales = 5; \
    int iOrder = 2; \
    double iAlpha = 1.0; \
    bool iEstimatedDistanceMean = false; \
    bool iNormalizedByFeatureStrength = false; \
    bool iDeltaFeatureStrength = false; \
    std::string inputVolume; \
    std::string outputVolume; \
    std::string featuresVolume; \
    std::string scalesImageVolume = "None"; \
    std::string strengthImageVolume = "None"; \
    bool echoSwitch = false; \
    bool xmlSwitch = false; \
    std::string processInformationAddressString = "0"; \
    std::string returnParameterFile; \
    std::string fullDescription("Description: "); \
    fullDescription += "This module implements a fast version of the popular Non-Local Means filter for image denoising. This algorithm filters each pixel as a weighted average of its neighbors in a large vicinity. The weights are computed based on the similarity of each neighbor with the voxel to be denoised.\n In the original formulation a patch with a certain radius is centered in each of the voxels, and the Mean Squared Error between each pair of corresponding voxels is computed. In this implementation, only the mean value and gradient components are compared. This, together with an efficient memory management, can attain a speed-up of nearly 20x. Besides, the filtering is more accurate than the original with poor SNR.\n This code is intended for its use with MRI (or any other Rician-distributed modality): the second order moment is estimated, then we subtract twice the squared power of noise, and finally we take the square root of the result to remove the Rician bias.\n The original implementation of the NLM filter may be found in:\n A. Buades, B. Coll, J. Morel, 'A review of image denoising algorithms, with a new one', Multiscale Modelling and Simulation 4(2): 490-530. 2005.\n The correction of the Rician bias is described in the following reference (among others):\n S. Aja-Fernandez, K. Krissian, 'An unbiased Non-Local Means scheme for DWI filtering', in: Proceedings of the MICCAI Workshop on Computational Diffusion MRI, 2008, pp. 277-284.\n The whole description of this version may be found in the following paper (please, cite it if you are willing to use this software):\n A. Tristan-Vega, V. Garcia Perez, S. Aja-Fenandez, and C.-F. Westin, 'Efficient and Robust Nonlocal Means Denoising of MR Data Based on Salient Features Matching', Computer Methods and Programs in Biomedicine. (Accepted for publication) 2011."; \
    if (!std::string("Antonio Tristan Vega, Veronica Garcia-Perez, Santiago Aja-Fernandez, Carl-Fredrik Westin").empty()) \
      { \
      fullDescription += "\nAuthor(s): Antonio Tristan Vega, Veronica Garcia-Perez, Santiago Aja-Fernandez, Carl-Fredrik Westin"; \
      } \
    if (!std::string("Supported by grant number FMECD-2010/71131616E from the Spanish Ministry of Education/Fulbright Committee").empty()) \
      { \
      fullDescription += "\nAcknowledgements: Supported by grant number FMECD-2010/71131616E from the Spanish Ministry of Education/Fulbright Committee"; \
      } \
    TCLAP::CmdLine commandLine (fullDescription, \
       ' ', \
      "0.0.1.$Revision: 1 $(beta)" ); \
 \
      itksys_ios::ostringstream msg; \
    msg.str("");msg << "The root power of noise (sigma) in the complex Gaussian process the Rician comes from. If it is underestimated, the algorithm fails to remove the noise. If it is overestimated, over-blurring is likely to occur. (default: " << iSigma << ")"; \
    TCLAP::ValueArg<float > iSigmaArg("", "sigma", msg.str(), 0, iSigma, "float", commandLine); \
 \
    msg.str("");msg << "The algorithm search for similar voxels in a neighborhood of this radius (radii larger than 5,5,5 are very slow, and the results can be only marginally better. Small radii may fail to effectively remove the noise). (default: " << iRadiusSearchTemp << ")"; \
    TCLAP::ValueArg<std::string > iRadiusSearchArg("", "rs", msg.str(), 0, iRadiusSearchTemp, "std::vector<int>", commandLine); \
 \
    msg.str("");msg << "Similarity between blocks is computed as the difference between mean values and gradients. These parameters are computed fitting a hyperplane with LS inside a neighborhood of this size (default: " << iRadiusCompTemp << ")"; \
    TCLAP::ValueArg<std::string > iRadiusCompArg("", "rc", msg.str(), 0, iRadiusCompTemp, "std::vector<int>", commandLine); \
 \
    msg.str("");msg << "This parameter is related to noise; the larger the parameter, the more aggressive the filtering. Should be near 1, and only values between 0.8 and 1.2 are allowed (default: " << iH << ")"; \
    TCLAP::ValueArg<float > iHArg("", "hp", msg.str(), 0, iH, "float", commandLine); \
 \
    msg.str("");msg << "To accelerate computations, preselection is used: if the normalized difference is above this threshold, the voxel will be discarded (non used for average) (default: " << iPs << ")"; \
    TCLAP::ValueArg<float > iPsArg("", "ps", msg.str(), 0, iPs, "float", commandLine); \
 \
    msg.str("");msg << "Minimum scale level (default: " << iMinScale << ")"; \
    TCLAP::ValueArg<double > iMinScaleArg("", "min", msg.str(), 0, iMinScale, "double", commandLine); \
 \
    msg.str("");msg << "Maximum scale level (default: " << iMaxScale << ")"; \
    TCLAP::ValueArg<double > iMaxScaleArg("", "max", msg.str(), 0, iMaxScale, "double", commandLine); \
 \
    msg.str("");msg << "Number of scales (default: " << iNScales << ")"; \
    TCLAP::ValueArg<int > iNScalesArg("", "nos", msg.str(), 0, iNScales, "int", commandLine); \
 \
    msg.str("");msg << " (default: " << iOrder << ")"; \
    TCLAP::ValueArg<int > iOrderArg("", "ord", msg.str(), 0, iOrder, "int", commandLine); \
 \
    msg.str("");msg << " (default: " << iAlpha << ")"; \
    TCLAP::ValueArg<double > iAlphaArg("", "alpha", msg.str(), 0, iAlpha, "double", commandLine); \
 \
    msg.str("");msg << "Use Estimated Distance Mean (default: " << iEstimatedDistanceMean << ")"; \
    TCLAP::SwitchArg iEstimatedDistanceMeanArg("", "estDist", msg.str(), commandLine, iEstimatedDistanceMean); \
 \
    msg.str("");msg << "Use Normalized By Feature Strength (default: " << iNormalizedByFeatureStrength << ")"; \
    TCLAP::SwitchArg iNormalizedByFeatureStrengthArg("", "normByFeat", msg.str(), commandLine, iNormalizedByFeatureStrength); \
 \
    msg.str("");msg << "Use Delta Feature Strength (default: " << iDeltaFeatureStrength << ")"; \
    TCLAP::SwitchArg iDeltaFeatureStrengthArg("", "deltaStr", msg.str(), commandLine, iDeltaFeatureStrength); \
 \
    msg.str("");msg << "Input MRI volume.";    TCLAP::ValueArg<std::string > inputVolumeArg("", "i", msg.str(), 0, inputVolume, "std::string", commandLine); \
 \
    msg.str("");msg << "Output (filtered) MRI volume.";    TCLAP::ValueArg<std::string > outputVolumeArg("", "o", msg.str(), 0, outputVolume, "std::string", commandLine); \
 \
    msg.str("");msg << "Features volume.";    TCLAP::ValueArg<std::string > featuresVolumeArg("", "feat", msg.str(), 0, featuresVolume, "std::string", commandLine); \
 \
    msg.str("");msg << "Scales volume. (default: " << scalesImageVolume << ")"; \
    TCLAP::ValueArg<std::string > scalesImageVolumeArg("", "scale", msg.str(), 0, scalesImageVolume, "std::string", commandLine); \
 \
    msg.str("");msg << "Strength volume. (default: " << strengthImageVolume << ")"; \
    TCLAP::ValueArg<std::string > strengthImageVolumeArg("", "strength", msg.str(), 0, strengthImageVolume, "std::string", commandLine); \
 \
    msg.str("");msg << "Echo the command line arguments (default: " << echoSwitch << ")"; \
    TCLAP::SwitchArg echoSwitchArg("", "echo", msg.str(), commandLine, echoSwitch); \
 \
    msg.str("");msg << "Produce xml description of command line arguments (default: " << xmlSwitch << ")"; \
    TCLAP::SwitchArg xmlSwitchArg("", "xml", msg.str(), commandLine, xmlSwitch); \
 \
    msg.str("");msg << "Address of a structure to store process information (progress, abort, etc.). (default: " << processInformationAddressString << ")"; \
    TCLAP::ValueArg<std::string > processInformationAddressStringArg("", "processinformationaddress", msg.str(), 0, processInformationAddressString, "std::string", commandLine); \
 \
    msg.str("");msg << "Filename in which to write simple return parameters (int, float, int-vector, etc.) as opposed to bulk return parameters (image, geometry, transform, measurement, table).";    TCLAP::ValueArg<std::string > returnParameterFileArg("", "returnparameterfile", msg.str(), 0, returnParameterFile, "std::string", commandLine); \
 \
try \
  { \
    /* Build a map of flag aliases to the true flag */ \
    std::map<std::string,std::string> flagAliasMap; \
    std::map<std::string,std::string> deprecatedFlagAliasMap; \
    std::map<std::string,std::string> longFlagAliasMap; \
    std::map<std::string,std::string> deprecatedLongFlagAliasMap; \
    /* Remap flag aliases to the true flag */ \
    std::vector<std::string> targs; \
    std::map<std::string,std::string>::iterator ait; \
    std::map<std::string,std::string>::iterator dait; \
    size_t ac; \
    for (ac=0; ac < static_cast<size_t>(argc); ++ac)  \
       {  \
       if (strlen(argv[ac]) == 2 && argv[ac][0]=='-') \
         { \
         /* short flag case */ \
         std::string tflag(argv[ac], 1, strlen(argv[ac])-1); \
         ait = flagAliasMap.find(tflag); \
         dait = deprecatedFlagAliasMap.find(tflag); \
         if (ait != flagAliasMap.end() || dait != deprecatedFlagAliasMap.end()) \
           { \
           if (ait != flagAliasMap.end()) \
             { \
             /* remap the flag */ \
             targs.push_back("-" + (*ait).second); \
             } \
           else if (dait != deprecatedFlagAliasMap.end()) \
             { \
             std::cout << "Flag \"" << argv[ac] << "\" is deprecated. Please use flag \"-" << (*dait).second << "\" instead. " << std::endl; \
             /* remap the flag */ \
             targs.push_back("-" + (*dait).second); \
             } \
           } \
         else \
           { \
           targs.push_back(argv[ac]); \
           } \
         } \
       else if (strlen(argv[ac]) > 2 && argv[ac][0]=='-' && argv[ac][1]=='-') \
         { \
         /* long flag case */ \
         std::string tflag(argv[ac], 2, strlen(argv[ac])-2); \
         ait = longFlagAliasMap.find(tflag); \
         dait = deprecatedLongFlagAliasMap.find(tflag); \
         if (ait != longFlagAliasMap.end() || dait != deprecatedLongFlagAliasMap.end()) \
           { \
           if (ait != longFlagAliasMap.end()) \
             { \
             /* remap the flag */ \
             targs.push_back("--" + (*ait).second); \
             } \
           else if (dait != deprecatedLongFlagAliasMap.end()) \
             { \
             std::cout << "Long flag \"" << argv[ac] << "\" is deprecated. Please use long flag \"--" << (*dait).second << "\" instead. " << std::endl; \
             /* remap the flag */ \
             targs.push_back("--" + (*dait).second); \
             } \
           } \
         else \
           { \
           targs.push_back(argv[ac]); \
           } \
         } \
       else if (strlen(argv[ac]) > 2 && argv[ac][0]=='-' && argv[ac][1]!='-') \
         { \
         /* short flag case where multiple flags are given at once ala */ \
         /* "ls -ltr" */ \
         std::string tflag(argv[ac], 1, strlen(argv[ac])-1); \
         std::string rflag("-"); \
         for (std::string::size_type fi=0; fi < tflag.size(); ++fi) \
           { \
           std::string tf(tflag, fi, 1); \
           ait = flagAliasMap.find(tf); \
           dait = deprecatedFlagAliasMap.find(tf); \
           if (ait != flagAliasMap.end() || dait != deprecatedFlagAliasMap.end()) \
             { \
             if (ait != flagAliasMap.end()) \
               { \
               /* remap the flag */ \
               rflag += (*ait).second; \
               } \
             else if (dait != deprecatedFlagAliasMap.end()) \
               { \
               std::cout << "Flag \"-" << tf << "\" is deprecated. Please use flag \"-" << (*dait).second << "\" instead. " << std::endl; \
               /* remap the flag */ \
               rflag += (*dait).second; \
               } \
             } \
           else \
             { \
             rflag += tf; \
             } \
           } \
         targs.push_back(rflag); \
         } \
       else \
         { \
         /* skip the argument without remapping (this is the case for any */ \
         /* arguments for flags */ \
         targs.push_back(argv[ac]); \
         } \
       } \
 \
   /* Remap args to a structure that CmdLine::parse() can understand*/ \
   std::vector<char*> vargs; \
   for (ac = 0; ac < targs.size(); ++ac) \
     {  \
     vargs.push_back(const_cast<char *>(targs[ac].c_str())); \
     } \
    commandLine.parse ( vargs.size(), (char**) &(vargs[0]) ); \
    iSigma = iSigmaArg.getValue(); \
    iRadiusSearchTemp = iRadiusSearchArg.getValue(); \
    iRadiusCompTemp = iRadiusCompArg.getValue(); \
    iH = iHArg.getValue(); \
    iPs = iPsArg.getValue(); \
    iMinScale = iMinScaleArg.getValue(); \
    iMaxScale = iMaxScaleArg.getValue(); \
    iNScales = iNScalesArg.getValue(); \
    iOrder = iOrderArg.getValue(); \
    iAlpha = iAlphaArg.getValue(); \
    iEstimatedDistanceMean = iEstimatedDistanceMeanArg.getValue(); \
    iNormalizedByFeatureStrength = iNormalizedByFeatureStrengthArg.getValue(); \
    iDeltaFeatureStrength = iDeltaFeatureStrengthArg.getValue(); \
    inputVolume = inputVolumeArg.getValue(); \
    outputVolume = outputVolumeArg.getValue(); \
    featuresVolume = featuresVolumeArg.getValue(); \
    scalesImageVolume = scalesImageVolumeArg.getValue(); \
    strengthImageVolume = strengthImageVolumeArg.getValue(); \
    echoSwitch = echoSwitchArg.getValue(); \
    xmlSwitch = xmlSwitchArg.getValue(); \
    processInformationAddressString = processInformationAddressStringArg.getValue(); \
    returnParameterFile = returnParameterFileArg.getValue(); \
      { /* Assignment for iRadiusSearch */ \
      std::vector<std::string> words; \
      std::string sep(","); \
      splitString(iRadiusSearchTemp, sep, words); \
      for (unsigned int _j = 0; _j < words.size(); _j++) \
        { \
        iRadiusSearch.push_back(atoi(words[_j].c_str())); \
        } \
      } \
      { /* Assignment for iRadiusComp */ \
      std::vector<std::string> words; \
      std::string sep(","); \
      splitString(iRadiusCompTemp, sep, words); \
      for (unsigned int _j = 0; _j < words.size(); _j++) \
        { \
        iRadiusComp.push_back(atoi(words[_j].c_str())); \
        } \
      } \
  } \
catch ( TCLAP::ArgException e ) \
  { \
  std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; \
  return ( EXIT_FAILURE ); \
  }
#define GENERATE_ECHOARGS \
if (echoSwitch) \
{ \
std::cout << "Command Line Arguments" << std::endl; \
std::cout << "    iSigma: " << iSigma << std::endl; \
std::cout << "    iRadiusSearch: "; \
for (unsigned int _i =0; _i < iRadiusSearch.size(); _i++) \
{ \
std::cout << iRadiusSearch[_i] << ", "; \
} \
std::cout <<std::endl; \
std::cout << "    iRadiusComp: "; \
for (unsigned int _i =0; _i < iRadiusComp.size(); _i++) \
{ \
std::cout << iRadiusComp[_i] << ", "; \
} \
std::cout <<std::endl; \
std::cout << "    iH: " << iH << std::endl; \
std::cout << "    iPs: " << iPs << std::endl; \
std::cout << "    iMinScale: " << iMinScale << std::endl; \
std::cout << "    iMaxScale: " << iMaxScale << std::endl; \
std::cout << "    iNScales: " << iNScales << std::endl; \
std::cout << "    iOrder: " << iOrder << std::endl; \
std::cout << "    iAlpha: " << iAlpha << std::endl; \
std::cout << "    iEstimatedDistanceMean: " << iEstimatedDistanceMean << std::endl; \
std::cout << "    iNormalizedByFeatureStrength: " << iNormalizedByFeatureStrength << std::endl; \
std::cout << "    iDeltaFeatureStrength: " << iDeltaFeatureStrength << std::endl; \
std::cout << "    inputVolume: " << inputVolume << std::endl; \
std::cout << "    outputVolume: " << outputVolume << std::endl; \
std::cout << "    featuresVolume: " << featuresVolume << std::endl; \
std::cout << "    scalesImageVolume: " << scalesImageVolume << std::endl; \
std::cout << "    strengthImageVolume: " << strengthImageVolume << std::endl; \
std::cout << "    echoSwitch: " << echoSwitch << std::endl; \
std::cout << "    xmlSwitch: " << xmlSwitch << std::endl; \
std::cout << "    processInformationAddressString: " << processInformationAddressString << std::endl; \
std::cout << "    returnParameterFile: " << returnParameterFile << std::endl; \
}
#define GENERATE_ProcessInformationAddressDecoding \
ModuleProcessInformation *CLPProcessInformation = 0; \
if (processInformationAddressString != "") \
{ \
sscanf(processInformationAddressString.c_str(), "%p", &CLPProcessInformation); \
}
#define PARSE_ARGS GENERATE_LOGO;GENERATE_XML;GENERATE_TCLAP;GENERATE_ECHOARGS;GENERATE_ProcessInformationAddressDecoding;