#include "MolPolIO.hh"

#include "G4SystemOfUnits.hh"

#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>

#include "G4ParticleDefinition.hh"

#include "MolPolDetectorHit.hh"
#include "MolPolEvent.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>


MolPolIO::MolPolIO(){
    fTree = NULL;
    InitializeTree();
    // Default filename
    strcpy(fFilename, "MolPolout.root");

    fFile = NULL;
}

MolPolIO::~MolPolIO(){
    if( fTree ){ delete fTree; }
    fTree = NULL;
    if( fFile ){ delete fFile; }
    fFile = NULL;
}

void MolPolIO::SetFilename(G4String fn){
    G4cout << "Setting output file to " << fn << G4endl;
    strcpy(fFilename, fn.data());
}

void MolPolIO::InitializeTree(){
    if( fFile ){
	fFile->Close();
	delete fFile;
    }

    fFile = new TFile(fFilename, "RECREATE");

    if( fTree ){ delete fTree; }

    fTree = new TTree("T", "Geant4 Moller Polarimetry Simulation");

    // Event information
    fTree->Branch("ev.pid",   &fEvPart_PID, "ev.pid/I");
    fTree->Branch("ev.vx",    &fEvPart_X,   "ev.vx/D");
    fTree->Branch("ev.vy",    &fEvPart_Y,   "ev.vy/D");
    fTree->Branch("ev.vz",    &fEvPart_Z,   "ev.vz/D");
    fTree->Branch("ev.p",     &fEvPart_P,   "ev.p/D");
    fTree->Branch("ev.px",    &fEvPart_Px,  "ev.px/D");
    fTree->Branch("ev.py",    &fEvPart_Py,  "ev.py/D");
    fTree->Branch("ev.pz",    &fEvPart_Pz,  "ev.pz/D");
    fTree->Branch("ev.th",    &fEvPart_Th,  "ev.th/D");
    fTree->Branch("ev.ph",    &fEvPart_Ph,  "ev.ph/D");


    // DetectorHit
    fTree->Branch("hit.n",    &fNDetHit,     "hit.n/I");
    fTree->Branch("hit.det",  &fDetHit_det,  "hit.det[hit.n]/I");
    fTree->Branch("hit.vid",  &fDetHit_id,   "hit.vid[hit.n]/I");

    fTree->Branch("hit.pid",  &fDetHit_pid,   "hit.pid[hit.n]/I");
    fTree->Branch("hit.trid", &fDetHit_trid,  "hit.trid[hit.n]/I");
    fTree->Branch("hit.mtrid",&fDetHit_mtrid, "hit.mtrid[hit.n]/I");
    fTree->Branch("hit.gen",  &fDetHit_gen,   "hit.gen[hit.n]/I");

    fTree->Branch("hit.x",    &fDetHit_X,   "hit.x[hit.n]/D");
    fTree->Branch("hit.y",    &fDetHit_Y,   "hit.y[hit.n]/D");
    fTree->Branch("hit.z",    &fDetHit_Z,   "hit.z[hit.n]/D");

    fTree->Branch("hit.px",   &fDetHit_Px,   "hit.px[hit.n]/D");
    fTree->Branch("hit.py",   &fDetHit_Py,   "hit.py[hit.n]/D");
    fTree->Branch("hit.pz",   &fDetHit_Pz,   "hit.pz[hit.n]/D");

    fTree->Branch("hit.vx",   &fDetHit_Vx,   "hit.vx[hit.n]/D");
    fTree->Branch("hit.vy",   &fDetHit_Vy,   "hit.vy[hit.n]/D");
    fTree->Branch("hit.vz",   &fDetHit_Vz,   "hit.vz[hit.n]/D");

    fTree->Branch("hit.vdx",   &fDetHit_Vdx,   "hit.vdx[hit.n]/D");
    fTree->Branch("hit.vdy",   &fDetHit_Vdy,   "hit.vdy[hit.n]/D");
    fTree->Branch("hit.vdz",   &fDetHit_Vdz,   "hit.vdz[hit.n]/D");

    fTree->Branch("hit.p",    &fDetHit_P,   "hit.p[hit.n]/D");
    fTree->Branch("hit.e",    &fDetHit_E,   "hit.e[hit.n]/D");
    fTree->Branch("hit.m",    &fDetHit_M,   "hit.m[hit.n]/D");

    return;
}

void MolPolIO::FillTree(){
    if( !fTree ){ 
	fprintf(stderr, "Error %s: %s line %d - Trying to fill non-existant tree\n", __PRETTY_FUNCTION__, __FILE__, __LINE__ );
	return; 
    }

    fTree->Fill();
}

void MolPolIO::Flush(){
    //  Set arrays to 0
    fNDetHit = 0;
}

void MolPolIO::WriteTree(){
    assert( fFile );
    assert( fTree );

    if( !fFile->IsOpen() ){
	G4cerr << "ERROR: " << __FILE__ << " line " << __LINE__ << ": TFile not open" << G4endl;
	exit(1);
    }

    G4cout << "Writing output to " << fFile->GetName() << "... ";

    fFile->cd();

    fTree->Write("T", TObject::kOverwrite);

    fTree->ResetBranchAddresses();
    delete fTree;
    fTree = NULL;

    fFile->Close();

    delete fFile;
    fFile = NULL;

    G4cout << "written" << G4endl;

    return;
}

///////////////////////////////////////////////////////////////////////////////
// Interfaces to output section ///////////////////////////////////////////////

// Event Data

void MolPolIO::SetEventData(MolPolEvent *ev){
    fEvPart_PID = ev->fPartType[0]->GetPDGEncoding();

    fEvPart_X = ev->fPartPos[0].x()/__L_UNIT;
    fEvPart_Y = ev->fPartPos[0].y()/__L_UNIT;
    fEvPart_Z = ev->fPartPos[0].z()/__L_UNIT;

    fEvPart_Px = ev->fPartMom[0].x()/__E_UNIT;
    fEvPart_Py = ev->fPartMom[0].y()/__E_UNIT;
    fEvPart_Pz = ev->fPartMom[0].z()/__E_UNIT;

    fEvPart_P = sqrt( fEvPart_Px * fEvPart_Px + fEvPart_Py * fEvPart_Py + fEvPart_Pz * fEvPart_Pz);

    fEvPart_Th = acos ( fEvPart_Pz / fEvPart_P );
    fEvPart_Ph = atan2( fEvPart_Py, fEvPart_Px );

    return;
}

// DetectorHit

void MolPolIO::AddDetectorHit(MolPolDetectorHit *hit){
    int n = fNDetHit;
    //printf("%d hits in detector\n", fNDetHit );

    if( n >= __IO_MAXHIT ){
//	G4cerr << "WARNING: " << __PRETTY_FUNCTION__ << " line " << __LINE__ << ":  Buffer size exceeded!" << G4endl;
	return;
    }

    fDetHit_det[n]  = hit->fDetID;
    fDetHit_id[n]   = hit->fCopyID;

    fDetHit_trid[n] = hit->fTrID;
    fDetHit_mtrid[n]= hit->fmTrID;
    fDetHit_pid[n]  = hit->fPID;
    fDetHit_gen[n]  = hit->fGen;

    fDetHit_X[n]  = hit->f3X.x()/__L_UNIT;
    fDetHit_Y[n]  = hit->f3X.y()/__L_UNIT;
    fDetHit_Z[n]  = hit->f3X.z()/__L_UNIT;

    fDetHit_Px[n]  = hit->f3P.x()/__E_UNIT;
    fDetHit_Py[n]  = hit->f3P.y()/__E_UNIT;
    fDetHit_Pz[n]  = hit->f3P.z()/__E_UNIT;

    fDetHit_Vx[n]  = hit->f3V.x()/__L_UNIT;
    fDetHit_Vy[n]  = hit->f3V.y()/__L_UNIT;
    fDetHit_Vz[n]  = hit->f3V.z()/__L_UNIT;

    fDetHit_Vdx[n]  = hit->f3D.x();
    fDetHit_Vdy[n]  = hit->f3D.y();
    fDetHit_Vdz[n]  = hit->f3D.z();

    fDetHit_P[n]  = hit->fP/__E_UNIT;
    fDetHit_E[n]  = hit->fE/__E_UNIT;
    fDetHit_M[n]  = hit->fM/__E_UNIT;

    fNDetHit++;

    return;
}

void MolPolIO::AddScintDetectorHit(MolPolScintDetectorHit *hit){

    return;
}





