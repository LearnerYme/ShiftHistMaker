#ifndef __YQAMAKER__
#define __YQAMAKER__

#include <iostream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"
#include "TString.h"

/*

    Define some detector macros here

*/


#include "StChain/StMaker.h"

#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"

// always use bTOF
#include "StPicoEvent/StPicoBTofPidTraits.h"

class StPicoDst;
class StPicoTrack;
class StPicoEvent;
class StPicoDstMaker;
class TH1F;
class TH2F;
class TH3F;
class TProfile;
class StPicoBTofPidTraits;
class StEpdGeom;
class StPicoEpdHit;
class StPicoETofPidTraits;

class StYQAMaker : public StMaker {
    public:
        StYQAMaker(const char* name, StPicoDstMaker* picoMaker, const char* outName);
        virtual ~StYQAMaker();
        virtual Int_t Init();
        virtual Int_t Make();
        virtual void Clear(Option_t* option = "");
        virtual Int_t Finish();

        Int_t MakeEvent();
        Int_t MakeTrack(const Int_t iTrack);
        Bool_t IsGoodTrigger();

        Double_t GetBTofBeta();

    private:

        TString mOutName;
        
        StPicoDstMaker* mPicoDstMaker;
        StPicoDst* mPicoDst;
        StPicoEvent* mPicoEvent;
        StPicoTrack* mPicoTrack;

        StPicoBTofPidTraits* mBTofPidTraits;
        
        Int_t mRunId;
        Double_t vx, vy, vz;
        
        // Histograms

        TH2F* h2NsigPTpc;
        TH3F* h3NsigYPtTpc;

        TH2F* h2NsigPTof;
        TH3F* h3NsigYPtTof;

        ClassDef(StYQAMaker,1)
};

#endif

