#include "DQMOffline/RecoB/interface/SoftLeptonTagPlotter.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DQMOffline/RecoB/interface/Tools.h"

#include <sstream>
#include <string>

using namespace std ;
using namespace RecoBTag;

static const string ordinal[9] = { "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th" };

SoftLeptonTagPlotter::SoftLeptonTagPlotter(const std::string & tagName,
					   const EtaPtBin & etaPtBin, const edm::ParameterSet& pSet, 
					   unsigned int mc, bool wf, DQMStore::IBooker & ibook) :
  BaseTagInfoPlotter(tagName, etaPtBin), mcPlots_(mc), willFinalize_(wf)
{
  const std::string softLepDir(theExtensionString.substr(1));
  
  if(willFinalize_) return;
  
  for (int i = 0; i < s_leptons; i++) {
    std::string s;
    s += ordinal[i] + " lepton ";
    m_leptonId.push_back(std::make_unique<FlavourHistograms<double>>(
						   s + "id",
						   "Lepton identification discriminaint",
						   60, -0.1, 1.1, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_leptonPt.push_back(std::make_unique<FlavourHistograms<double>>(
						   s + "pT",
						   "Lepton transverse moementum",
						   100, 0.0, 20.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_sip2dsig.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "sip2dsig",
        "Lepton signed 2D impact parameter significance",
        100, -20.0, 30.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_sip3dsig.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "sip3dsig",
        "Lepton signed 3D impact parameter significance",
        100, -20.0, 30.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_sip2d.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "sip2d",
        "Lepton signed 2D impact parameter",
        100, -20.0, 30.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_sip3d.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "sip3d",
        "Lepton signed 3D impact parameter",
        100, -20.0, 30.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_ptRel.push_back(std::make_unique<FlavourHistograms<double>>(
        s +  "pT rel",
        "Lepton transverse momentum relative to jet axis",
        100, 0.0, 10.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_p0Par.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "p0 par",
        "Lepton moementum along jet axis in the B rest frame",
        100, 0.0, 10.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_etaRel.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "eta rel",
        "Lepton pseudorapidity relative to jet axis",
        100, -5.0, 25.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_deltaR.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "delta R",
        "Lepton pseudoangular distance from jet axis",
        100, 0.0, 0.6, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_ratio.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "energy ratio",
        "Ratio of lepton momentum to jet energy",
        100, 0.0, 2.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
    m_ratioRel.push_back(std::make_unique<FlavourHistograms<double>>(
        s + "parallel energy ratio",
        "Ratio of lepton momentum along the jet axis to jet energy",
        100, 0.0, 2.0, false, false, true, "b", softLepDir, mcPlots_, ibook));
  }
}

SoftLeptonTagPlotter::~SoftLeptonTagPlotter() {}

void SoftLeptonTagPlotter::analyzeTag( const reco::BaseTagInfo * baseTagInfo, double jec, int jetFlavour, float w/*=1*/)
{
  const reco::SoftLeptonTagInfo * tagInfo = 
	dynamic_cast<const reco::SoftLeptonTagInfo *>(baseTagInfo);

  if (!tagInfo) {
    throw cms::Exception("Configuration")
      << "BTagPerformanceAnalyzer: Extended TagInfo not of type SoftLeptonTagInfo. " << endl;
  }

  int n_leptons = tagInfo->leptons();

  for (int i = 0; i != n_leptons && i != s_leptons; ++i) {
    const reco::SoftLeptonProperties& properties = tagInfo->properties(i);
    m_leptonPt[i]->fill(jetFlavour, tagInfo->lepton(i)->pt(), w);
    m_leptonId[i]->fill(jetFlavour, properties.quality(), w);
    m_sip2dsig[i]->fill(jetFlavour, properties.sip2dsig, w);
    m_sip3dsig[i]->fill(jetFlavour, properties.sip3dsig, w);
    m_sip2d[i]->fill(jetFlavour, properties.sip2d, w);
    m_sip3d[i]->fill(jetFlavour, properties.sip3d, w);
    m_ptRel[i]->fill(jetFlavour, properties.ptRel, w);
    m_p0Par[i]->fill(jetFlavour, properties.p0Par, w);
    m_etaRel[i]->fill(jetFlavour, properties.etaRel, w);
    m_deltaR[i]->fill(jetFlavour, properties.deltaR, w);
    m_ratio[i]->fill(jetFlavour, properties.ratio, w);
    m_ratioRel[i]->fill(jetFlavour, properties.ratioRel, w);
  }
}

void SoftLeptonTagPlotter::psPlot(const std::string & name)
{
  if(willFinalize_) return;

  const std::string cName("SoftLeptonPlots" + theExtensionString);
  setTDRStyle()->cd();
  TCanvas canvas(cName.c_str(), cName.c_str(), 600, 900);
  canvas.UseCurrentStyle();
  canvas.Divide(2,3);
  canvas.Print((name + cName + ".ps[").c_str());
  for (int i = 0; i < s_leptons; i++) {
    canvas.cd(1)->Clear();
    m_leptonId[i]->plot();
    canvas.cd(2)->Clear();
    m_leptonPt[i]->plot();
    canvas.cd(3)->Clear();
    m_sip2d[i]->plot();
    canvas.cd(4)->Clear();
    m_sip3d[i]->plot();
    canvas.cd(5)->Clear();
    m_sip2dsig[i]->plot();
    canvas.cd(6)->Clear();
    m_sip3dsig[i]->plot();
    canvas.Print((name + cName + ".ps").c_str());

    canvas.cd(1)->Clear();
    m_etaRel[i]->plot();
    canvas.cd(2)->Clear();
    m_deltaR[i]->plot();
    canvas.cd(3)->Clear();
    m_ratio[i]->plot();
    canvas.cd(4)->Clear();
    m_ratioRel[i]->plot();
    canvas.cd(5)->Clear();
    m_ptRel[i]->plot();
    canvas.cd(6)->Clear();
    m_p0Par[i]->plot();
    canvas.Print((name + cName + ".ps").c_str());
  }
  canvas.Print((name + cName + ".ps]").c_str());
}


void SoftLeptonTagPlotter::epsPlot(const std::string & name)
{
  if(willFinalize_) return;

  for (int i=0; i != s_leptons; ++i) {
    m_leptonId[i]->epsPlot( name );
    m_leptonPt[i]->epsPlot( name );
    m_sip2d[i]->epsPlot( name );
    m_sip3d[i]->epsPlot( name );
    m_sip2dsig[i]->epsPlot( name );
    m_sip3dsig[i]->epsPlot( name );
    m_ptRel[i]->epsPlot( name );
    m_p0Par[i]->epsPlot( name );
    m_etaRel[i]->epsPlot( name );
    m_deltaR[i]->epsPlot( name );
    m_ratio[i]->epsPlot( name );
    m_ratioRel[i]->epsPlot( name );
  }
}

