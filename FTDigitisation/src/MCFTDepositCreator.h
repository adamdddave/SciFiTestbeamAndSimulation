/** @class MCFTDepositCreator MCFTDepositCreator.h
 *
 *  From the list of MCHits, this algorithm fill a container of FTChannelID 
 *  with the total energy deposited in each of them, and store it in the 
 *  transient data store.
 *
 *
 *  @author COGNERAS Eric
 *  @date   2012-06-05
 */

#ifndef MCFTDEPOSITCREATOR_H 
#define MCFTDEPOSITCREATOR_H 1

// Include files
/// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// from Event
#include "Event/MCHit.h"
#include "Event/MCFTDeposit.h"
// LHCbKernel
#include "Kernel/FTChannelID.h"

#include "IMCFTAttenuationTool.h"
#include "IMCFTDepositPositionTool.h"

class MCFTDepositCreator : public GaudiHistoAlg {

  typedef std::pair<LHCb::FTChannelID, double> FTDoublePair;
  typedef std::vector< FTDoublePair > FTDoublePairs;

  typedef std::map<LHCb::FTChannelID, LHCb::MCFTDeposit*> FTchanDepMap;
  typedef std::pair<LHCb::FTChannelID, LHCb::MCFTDeposit*> FTchanDep;

public: 
  /// Standard constructor
  MCFTDepositCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDepositCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


  

private:

  inline double calcTimeToSiPM(const LHCb::MCHit* ftHit, const double releaseTime, const int iSpill) const {
    return ftHit->time() + (m_yMax - fabs(ftHit->midPoint().y())) * m_fiberPropagationTime + releaseTime + m_spillTimes[iSpill];
  }
  
  inline double calcTimeRefToSiPM(const LHCb::MCHit* ftHit, const double releaseTime, const int iSpill) const {
    return ftHit->time() + (m_yMax + fabs(ftHit->midPoint().y())) * m_fiberPropagationTime + releaseTime + m_spillTimes[iSpill];
  }
  
  /*
  inline double calcAtt(const double fracX, const double fracY, const int kx, const int ky) const{
    return fracX * ( fracY     * m_transmissionMap[m_nYSteps*(kx+1)+ky+1] + 
                     ( 1-fracY ) * m_transmissionMap[m_nYSteps*(kx+1)+ky]   ) +
      (1-fracX) * ( fracY     * m_transmissionMap[m_nYSteps*kx+ky+1] + 
                    (1-fracY) * m_transmissionMap[m_nYSteps*kx+ky]   );
  }
  
  inline double calcAttRef(const double fracX, const double fracY, const int kx, const int ky)const{
    return fracX * ( fracY     * m_transmissionRefMap[m_nYSteps*(kx+1)+ky+1] + 
                     ( 1-fracY ) * m_transmissionRefMap[m_nYSteps*(kx+1)+ky]   ) +
      (1-fracX) * ( fracY     * m_transmissionRefMap[m_nYSteps*kx+ky+1] + 
                    (1-fracY) * m_transmissionRefMap[m_nYSteps*kx+ky]   );
  }
  */

  StatusCode hitToChannelConversion(LHCb::MCHit* ftHit, LHCb::MCFTDeposits *depositCont, const unsigned int iSpill);
  //void calculateAttenuation(const LHCb::MCHit* ftHit, double& att, double& attRef);
  //void calculateTransmissionMap();
  void plotChannelProperties( const DeFTFibreMat* pL, FTDoublePairs channels, const LHCb::MCHit* ftHit);
  void addNoiseDeposit(LHCb::FTChannelID noiseChannel, double Energy, double TOA, int type, int npe, LHCb::MCFTDeposits* depositCont);
  std::pair<int,double> addXtalk(double Energy);
  


  IMCFTAttenuationTool* m_attenuationTool;
  std::string m_attenuationToolName;

  IMCFTDepositPositionTool* m_depositPositionTool;
  std::string m_depositPositionToolName;
  
  // Locations
  std::string m_inputLocation;     ///< FT MCHits Location
  std::string m_outputLocation;    ///< FT energy deposit Location

  // Spill properties     
  std::vector<std::string> m_spillNames;  ///< Vector of spill names and mc hit path
  std::vector<std::string> m_spillVector; ///< Vector of spill names
  std::vector<double> m_spillTimes;       ///< Vector of spill arrival times       

  // Fibre properties
  double       m_fiberPropagationTime;   ///< Light propagation time in fiber
  double       m_scintillationDecayTime; ///< Decay time of scintillation light release
 
  double       m_putMCParticlePcut;
  bool         m_killSignal;
  
  DeFTDetector* m_deFT; ///< pointer to FT detector description
  Rndm::Numbers m_flatDist; ///< random number generator 
  Rndm::Numbers m_rndmLandau;
  
  bool         m_useAttenuation;           ///< Use an attenuation map to attenuate the light according to a model?

  double       m_yMax;
  unsigned int m_numLayers;

  bool         m_simulateNoise;
  double       m_thermalNoiseRate;
  int          m_rdwindows;
  double       m_crossTalkProb;
  double       m_afterpulseProb;
  double       m_photoElectronsPerMeV;
  
  
  
  // Set geometry
  int Nlayers   = 4*3;  // layers * stations
  int NModule   = 12;   // per up/down
  int NMat      = 2;    // up and down
  int Nsipms    = 16;   // per module
  int Nchannels = 128;  // per sipm array
  int Ntotchannels = Nlayers * NModule * NMat * Nsipms * Nchannels;

  

};
#endif // MCFTDEPOSITCREATOR_H
