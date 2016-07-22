#include "MolPolPrimaryGeneratorAction.hh"
#include "MolPolMessenger.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "MolPolIO.hh"
//#include <iostream>

//using namespace std;

MolPolPrimaryGeneratorAction::MolPolPrimaryGeneratorAction()
  :rndmFlag("off")
{
  //cout << "MolPolPrimaryGeneratorAction::MolPolPrimaryGeneratorAction" << endl;
  G4int n_particle = 1;
  particleGun  = new G4ParticleGun(n_particle);
  
  //create a messenger for this class
  //gunMessenger = new MolPolMessenger(this);

  // default particle kinematic

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName="e-");
  particleGun->SetParticleDefinition(particle);
  angle = 0;
  //particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
  particleGun->SetParticleEnergy(1.063*GeV);
  //G4double position = -0.5*(Detector->GetWorldSizeX());
  
  //particleGun->SetParticlePosition(G4ThreeVector(.1*cm,.1*cm,0.*cm));

  fXmin = -5.0*mm;
  fXmax =  5.0*mm;

  fYmin = -5.*mm;
  fYmax =  5.*mm;

  fthetaMin = 0.0*deg;
  fthetaMax = 10.0*deg;  

  fphiMin = 0.0*deg;
  fphiMax = 360.0*deg;  

  fZ = 0.0;

  fDefaultEvent = new MolPolEvent();

}

MolPolPrimaryGeneratorAction::~MolPolPrimaryGeneratorAction()
{
  delete particleGun;
  delete gunMessenger;
  delete fDefaultEvent;
}
void MolPolPrimaryGeneratorAction::rand(){ 
}
void MolPolPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fDefaultEvent->Reset();
  //particleGun->SetParticlePosition(G4ThreeVector(.1*cm,.1*cm,0.*cm));
  //particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
  //G4ThreeVector pos = Simulation::getInstance()->detector->randPos();

  double xPos     = CLHEP::RandFlat::shoot( fXmin, fXmax );
  double yPos     = CLHEP::RandFlat::shoot( fYmin, fYmax );
  double zPos     = fZ;
  double thetaPos = CLHEP::RandFlat::shoot( fthetaMin, fthetaMax );
  double phiPos   = CLHEP::RandFlat::shoot( fphiMin, fphiMax );
  
  double pX = cos( phiPos ) * sin( thetaPos );
  double pY = sin( phiPos ) * sin( thetaPos );
  double pZ = cos( thetaPos );

  //particleGun->SetParticlePosition(pos);
  
  particleGun->SetParticlePosition( G4ThreeVector(xPos, yPos, zPos) );
  particleGun->SetParticleMomentumDirection( G4ThreeVector( pX, pY, pZ ) );


  fDefaultEvent->ProduceNewParticle(G4ThreeVector(xPos, yPos, zPos),
				    G4ThreeVector(pX, pY, pZ ),
				    particleGun->GetParticleDefinition()->GetParticleName() );
  fIO->SetEventData(fDefaultEvent);
  particleGun->GeneratePrimaryVertex(anEvent);    

}

void MolPolPrimaryGeneratorAction::SourceModeSet(G4int i) {
//      SourceModeSet(0); // point to the one below with default settings = 0. // should I just use default parameters?
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
