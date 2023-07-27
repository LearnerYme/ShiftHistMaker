#include "StYQAMaker.h"

#include "TMath.h"
#include "TVector3.h"

#include "phys_constants.h"
#include "StThreeVector.hh"

#include "StBTofUtil/tofPathLength.hh"

#include "DbConf.h"

ClassImp(StYQAMaker);

StYQAMaker::StYQAMaker(const char* name, StPicoDstMaker* picoMaker, const char* outName) : StMaker(name) {

    /*
        The construction function of this class.
        Do some of the initialization.
    */

    mPicoDstMaker = picoMaker;
    mPicoDst = 0;
    mPicoEvent = 0;
    mPicoTrack = 0;

    TString fileName = outName;
    mOutName = fileName + ".root";

    #ifdef __WITH_EPD__
        mEpdGeom = new StEpdGeom();
    #endif

}

StYQAMaker::~StYQAMaker() {
    
    /*
        The deconstruction function.
        Need to do nothing.
    */

}

Int_t StYQAMaker::Init() {
    
    /*
        Init function.
    */
   
    std::cout << "[LOG] - From Init: " << "Reading run index of this data base." << std::endl;
    const Int_t nRuns = DbConf::mRunIdxMap.size();
    std::cout << "[LOG] - From Init: " << "There will be " << nRuns << " runs, starts with " << DbConf::mRunIdxMap.begin()->first << " and end up with " << DbConf::mRunIdxMap.end()->first << "." << std::endl;

    std::cout << "[LOG] - From Init: " << "Reading valid trigger ID(s) ." << std::endl;
    const Int_t nTrgs = DbConf::mTriggers.size();
    std::cout << "[LOG] - From Init: " << "There is/are " << nTrgs << " MB trigger(s): ";
    for (const UInt_t& trg :  DbConf::mTriggers) {
        std::cout << "\t" << trg;
    }
    std::cout << "." << std::endl;

    std::cout << "[LOG] - From Init: " << "Initializing Histograms." << std::endl;
    
    // negative p or pT means negative charge

    h2NsigPTpc = new TH2F(
        "h2NsigPTpc", "TPC;n#sigma;p [GeV/c]", 
        400, -20, 20,
        80, -4.0, 4.0 
    );
    h3NsigYPtTpc = new TH3F(
        "h2NsigYPtTpc", "TPC;n#sigma;y;p_{T} [GeV/c]",
        400, -20, 20, 
        14, -0.7, 0.7, 
        80, -4.0, 4.0 
    );

    h2NsigPTof = new TH2F(
        "h2NsigPTof", "TOF;n#sigma;p [GeV/c]", 
        400, -20, 20,
        80, -4.0, 4.0 
    );

    h3NsigYPtTof = new TH3F(
        "h2NsigYPtTof", "TOF;n#sigma;y;p_{T} [GeV/c]",
        400, -20, 20, 
        14, -0.7, 0.7, 
        80, -4.0, 4.0 
    );

    std::cout << "[LOG] - From Init: " << "This is the end of Init() function." << std::endl;

    return kStOK;
}

Int_t StYQAMaker::Finish() {

    /*
        Finish function, write histograms or profiles into root file.
        And save the file.
    */

    std::cout << "[LOG] - From Finish: " << "Creating root file." << std::endl;
    TFile* tfout = new TFile(mOutName, "recreate");
    std::cout << "[LOG] - From Finish: " << mOutName.Data() << std::endl;
    tfout->cd();

    h2NsigPTpc->Write();
    h2NsigPTof->Write();
    h3NsigYPtTpc->Write();
    h3NsigYPtTof->Write();

    tfout->Close();
    std::cout << "[LOG] - From Finish: " << ".root file saved." << std::endl;

    return kStOK;

}

Int_t StYQAMaker::Make() {

    /*
        Do MakeEvent().
    */

    MakeEvent();
    return kStOK;

}

void StYQAMaker::Clear(Option_t* option) {

    /*
        Clear function.
        Set pointers to null.
    */

    mPicoDstMaker = 0;
    mPicoEvent = 0;
    mPicoTrack = 0;
    
    mBTofPidTraits = 0;
}

Int_t StYQAMaker::MakeEvent() {

    /*
        To make an event.
    */

    mPicoEvent = mPicoDst->event();
    if (!mPicoEvent) {
        LOG_WARN << "[WARNING] - From MakeEvent: " << "Can not open PicoDst event, skip this." << endm;
        return kStWarn;
    }
    
    if (!IsGoodTrigger()) { // only use MB triggers
        return kStOK;
    }

    Int_t runRawID = mPicoEvent->runId();
    if (DbConf::mRunIdxMap.count(runRawID) == 0) {
        LOG_WARN << "[WARNING] - From MakeEvent: " << runRawID << " is not a valid run from the run list." << endm;
        return kStWarn;
    }
    mRunId = DbConf::mRunIdxMap.at(runRawID);

    TVector3 vertex = mPicoEvent->primaryVertex();
    vx = vertex.X();
    vy = vertex.Y();
    vz = vertex.Z();
    Double_t vr = vertex.Perp();

    if (vr > 2.0 || fabs(vz) > 50.0) { // do the vr and vz cut
        return kStOK;
    }
 

    // Make TPC and TOF issue
    const int numberOfTracks = mPicoDst->numberOfTracks();
    for (Int_t iTrack=0; iTrack<numberOfTracks; iTrack++) {
        MakeTrack(iTrack);
    }

    return kStOK;

}

Int_t StYQAMaker::MakeTrack(Int_t iTrack) {

    /*
        A block of processing the TPC tracks.
        Note that, the iTrack should be from looping of tracks.
    */

    mPicoTrack = mPicoDst->track(iTrack);
    if (!mPicoTrack) {
        LOG_WARN << "[WARNING] - From MakeTrack: " << "Can not open picoTrack from pidoDst, skip." << endm;
        return kStWarn;
    }

    if (!mPicoTrack->isPrimary()) {
        return kStOK;
    }

    if (mPicoTrack->isTofTrack()) {
        mBTofPidTraits = mPicoDst->btofPidTraits(mPicoTrack->bTofPidTraitsIndex());
    } else {
        mBTofPidTraits = 0;
    }

    Double_t DCA = mPicoTrack->gDCA(vx, vy, vz);
    
    Int_t nHitsFit = mPicoTrack->nHitsFit();
    Int_t nHitsPoss = mPicoTrack->nHitsPoss();
    Float_t nHitsRatio = 1.0 * nHitsFit / nHitsPoss;
    Int_t nHitsDedx = mPicoTrack->nHitsDedx();
    Int_t q = mPicoTrack->charge();

    Double_t nSigProton = mPicoTrack->nSigmaProton();

    // here use quality cut to remove bad tracks
    if (DCA > 1.0 || nHitsFit <= 15 || nHitsDedx < 5 || nHitsRatio < 0.52) {
        return kStOK;
    }

    TVector3 momentum = mPicoTrack->pMom();
    Double_t pt = momentum.Perp();
    Double_t p = momentum.Mag();
    Double_t pz = momentum.Z();
    Double_t EP = sqrt(p*p + 0.938272 * 0.938272);
    Double_t YP = TMath::Log((EP + pz) / (EP - pz + 1e-7)) * 0.5; 


    Double_t bTofBeta = GetBTofBeta();
    
    Double_t rigi = p * q;
    Double_t rigiT = pt * q;

    Double_t m2b = -999;

    if (bTofBeta > 1e-5) {
        m2b = p*p * (pow(bTofBeta, -2.0) - 1);
    }

    h2NsigPTpc->Fill(nSigProton, rigi);
    h3NsigYPtTpc->Fill(nSigProton, YP, rigiT);

    if (m2b > 0.6 && m2b < 1.2) {
        h2NsigPTof->Fill(nSigProton, rigi);
        h3NsigYPtTof->Fill(nSigProton, YP, rigiT);
    }

    return kStOK;
}

Bool_t StYQAMaker::IsGoodTrigger() {
    for (const UInt_t& trg : DbConf::mTriggers) {
        if (mPicoEvent->isTrigger(trg)) {
            return kTRUE;
        }
    }
    return kFALSE;
}

Double_t StYQAMaker::GetBTofBeta() {
    if (!mPicoTrack->isTofTrack() || !mBTofPidTraits || mBTofPidTraits->btofMatchFlag() <= 0) {
        return 0.0;
    }
    Double_t beta = mBTofPidTraits->btofBeta();
    return beta < 1e-5 ?  0.0 : beta;
}
