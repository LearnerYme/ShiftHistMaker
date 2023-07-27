#include <TSystem>

class StMaker;
class StChain;
class StPicoDstMaker;
class StPicoEvent;
class StYQAMaker;

StChain *chain;

void readPicoDst(const TString inputFile = "file.list",
                 const TString outputFile = "qaOut") {
   	Int_t nEvents = 15000000;

  	gROOT->LoadMacro(
    	"$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C"
	);
  	loadSharedLibraries();

	gSystem->Load("StChain");
	gSystem->Load("StUtilities");
	gSystem->Load("StBichsel");
	gSystem->Load("StPicoEvent");
	gSystem->Load("StPicoDstMaker");
	gSystem->Load("StTofUtil");
	gSystem->Load("StBTofUtil");
	gSystem->Load("StETofUtil");
	gSystem->Load("StEpdUtil");

	gSystem->Load("StYQAMaker");

	chain = new StChain();

	StPicoDstMaker *picoMaker = new StPicoDstMaker(2, inputFile, "picoDst");
	StYQAMaker *anaMaker = new StYQAMaker("ana", picoMaker, outputFile);

	chain->Init();
	cout << "chain->Init();" << endl;
	int total = picoMaker->chain()->GetEntries();
	cout << " Total entries = " << total << endl;
	if (nEvents > total) nEvents = total;
	for (Int_t i = 0; i < nEvents; i++) {
		if (i % 1000 == 0) {
			cout << "Working on eventNumber " << i << endl; 
		}

		chain->Clear();
		int iret = chain->Make(i);

		if (iret) {
			cout << "Bad return code!" << iret << endl;
			break;
		}

		total++;
	}

	cout << "****************************************** " << endl;
	cout << "Work done... now its time to close up shop!" << endl;
	cout << "****************************************** " << endl;
	
	chain->Finish();
	
	cout << "****************************************** " << endl;
	cout << "total number of events  " << nEvents << endl;
	cout << "****************************************** " << endl;

	delete chain;
}
