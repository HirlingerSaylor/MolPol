// ********************************************************************
//
// $Id: MolPolEMField.hh,v 1.0, 2010/12/26 MolPol Exp $
// GEANT4 tag $Name: geant4-09-04 $
//
//
//   User Field class Setup implementation.
//
#include "MolPolEMFieldSetup.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4EquationOfMotion.hh"
#include "G4EqMagElectricField.hh"
#include "MolPolQuad.hh"
#include "MolPolDipole.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"
#include "G4ios.hh"

#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////////
MolPolEMFieldSetup* MolPolEMFieldSetup::fMolPolEMFieldSetup=0;
MolPolEMFieldSetup* MolPolEMFieldSetup::GetMolPolEMFieldSetup()
{ 
	if(!fMolPolEMFieldSetup)  
	{
		G4cout<<"MolPolEMFieldSetup is not initialized yet...exit...\n";
		exit(-99);
	}
	return fMolPolEMFieldSetup; 
}

//////////////////////////////////////////////////////////////////////////
//
MolPolEMFieldSetup::MolPolEMFieldSetup()
: fChordFinder(0), fStepper(0), fIntgrDriver(0)
{
  fMolPolEMFieldSetup=this;

  fEMfield = new MolPolEMField();
  fEquation = new G4EqMagElectricField(fEMfield);
  fMinStep  = 0.00001*mm ; // minimal step of 1 miron, default is 0.01 mm, Nickie finely
  fStepperType = 4 ;     // ClassicalRK4 -- the default stepper
  
  fFieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  
  fChordFinder = 0;   //will be set in UpdateField()
  UpdateField();

  G4RotationMatrix* NOROT = new G4RotationMatrix;

  //PREX setting: 1.06GeV 2.,0.180542,0.0407379,0.0,0.0445758,0.18
  
  G4double ORIGINQ1 = 75.19 * cm;
  G4double ORIGINQ2 =140.46 * cm;
  G4double ORIGINQ3 =209.59 * cm;
  G4double ORIGINQ4 =274.59 * cm;
  G4double ORIGIND  =0 * cm;

  G4double Q1R= 5.08 * cm;
  G4double Q2R= 5.08 * cm;
  G4double Q3R= 5.08 * cm;
  G4double Q4R= 5.08 * cm;
  
  G4double KAPPA1 =  0.180542  * tesla / Q1R;
  G4double KAPPA2 =  0.0407379 * tesla / Q2R;
  G4double KAPPA3 =  0.0       * tesla / Q3R;
  G4double KAPPA4 =  0.0445758 * tesla / Q4R;
  G4double DIPOLE =  0.18      * tesla;

  G4cout << "Magnet settings: " << KAPPA1 << " " << KAPPA2 << " " << KAPPA3 << " " << KAPPA4 << " " << DIPOLE << G4endl;

  fMagFieldFZB1 = new MolPolQuad(KAPPA1, G4ThreeVector(0.0, 0.0, ORIGINQ1), NOROT, Q1R);
  fEquationFZB1 = new G4Mag_UsualEqRhs(fMagFieldFZB1);	
  fStepperFZB1  = new G4ClassicalRK4(fEquationFZB1);
  fLocalFieldManagerFZB1 = new G4FieldManager();
  fChordFinderFZB1 = 0;
  UpdateFieldFZB1();
  fMagFieldFZB2 = new MolPolQuad(KAPPA2, G4ThreeVector(0.0, 0.0, ORIGINQ2), NOROT, Q2R);
  fEquationFZB2 = new G4Mag_UsualEqRhs(fMagFieldFZB2);	
  fStepperFZB2  = new G4ClassicalRK4(fEquationFZB2);
  fLocalFieldManagerFZB2 = new G4FieldManager();
  fChordFinderFZB2 = 0;
  UpdateFieldFZB2();
  fMagFieldFZB3 = new MolPolQuad(KAPPA3, G4ThreeVector(0.0, 0.0, ORIGINQ3), NOROT, Q3R);
  fEquationFZB3 = new G4Mag_UsualEqRhs(fMagFieldFZB3);	
  fStepperFZB3  = new G4ClassicalRK4(fEquationFZB3);
  fLocalFieldManagerFZB3 = new G4FieldManager();
  fChordFinderFZB3 = 0;
  UpdateFieldFZB3();
  fMagFieldFZB4 = new MolPolQuad(KAPPA4, G4ThreeVector(0.0, 0.0, ORIGINQ4), NOROT, Q4R);
  fEquationFZB4 = new G4Mag_UsualEqRhs(fMagFieldFZB4);	
  fStepperFZB4  = new G4ClassicalRK4(fEquationFZB4);
  fLocalFieldManagerFZB4 = new G4FieldManager();
  fChordFinderFZB4 = 0;
  UpdateFieldFZB4();

  fMagFieldFZB5 = new MolPolDipole(DIPOLE, G4ThreeVector(0.0, 0.0, ORIGIND), NOROT);
  fEquationFZB5 = new G4Mag_UsualEqRhs(fMagFieldFZB5);
  fStepperFZB5  = new G4ClassicalRK4(fEquationFZB5);
  fLocalFieldManagerFZB5 = new G4FieldManager();
  fChordFinderFZB5 = 0;
  UpdateFieldFZB5();
    
}

/////////////////////////////////////////////////////////////////////////////////
//

MolPolEMFieldSetup::~MolPolEMFieldSetup()
{
	if(fChordFinder) delete fChordFinder;
	if(fStepper)     delete fStepper;
	if(fEquation)    delete fEquation;
	if(fEMfield)     delete fEMfield;
}

/////////////////////////////////////////////////////////////////////////////
//
// Register this field to 'global' Field Manager and
// Create Stepper and Chord Finder with predefined type, minstep (resp.)
//

void MolPolEMFieldSetup::UpdateField()
{
  fStepper = new G4ClassicalRK4( fEquation, 8 );
  
  fFieldManager->SetDetectorField(fEMfield);
  
  if(fChordFinder) delete fChordFinder;
  fIntgrDriver = new G4MagInt_Driver(fMinStep,fStepper,fStepper->GetNumberOfVariables());
  fChordFinder = new G4ChordFinder(fIntgrDriver);
  fFieldManager->SetChordFinder( fChordFinder );  
}


/////////////////////////////////////////////////////////////////////////////
void MolPolEMFieldSetup::UpdateFieldFZB1()
{

	fLocalFieldManagerFZB1->SetDetectorField(fMagFieldFZB1);

	if(fChordFinderFZB1) delete fChordFinderFZB1;
	fIntgrDriverFZB1 = new G4MagInt_Driver(fMinStep,fStepperFZB1,fStepperFZB1->GetNumberOfVariables());
	fChordFinderFZB1 = new G4ChordFinder((G4MagneticField*) fMagFieldFZB1, fMinStep, fStepperFZB1);
	fLocalFieldManagerFZB1->SetChordFinder( fChordFinderFZB1 );
	
}

/////////////////////////////////////////////////////////////////////////////
void MolPolEMFieldSetup::UpdateFieldFZB2()
{

	fLocalFieldManagerFZB2->SetDetectorField(fMagFieldFZB2);

	if(fChordFinderFZB2) delete fChordFinderFZB2;
	fIntgrDriverFZB2 = new G4MagInt_Driver(fMinStep,fStepperFZB2,fStepperFZB2->GetNumberOfVariables());
	fChordFinderFZB2 = new G4ChordFinder((G4MagneticField*) fMagFieldFZB2, fMinStep, fStepperFZB2);
	fLocalFieldManagerFZB2->SetChordFinder( fChordFinderFZB2 );
	
}

/////////////////////////////////////////////////////////////////////////////
void MolPolEMFieldSetup::UpdateFieldFZB3()
{

	fLocalFieldManagerFZB3->SetDetectorField(fMagFieldFZB3);

	if(fChordFinderFZB3) delete fChordFinderFZB3;
	fIntgrDriverFZB3 = new G4MagInt_Driver(fMinStep,fStepperFZB3,fStepperFZB3->GetNumberOfVariables());
	fChordFinderFZB3 = new G4ChordFinder((G4MagneticField*) fMagFieldFZB3, fMinStep, fStepperFZB3);
	fLocalFieldManagerFZB3->SetChordFinder( fChordFinderFZB3 );
	
}

/////////////////////////////////////////////////////////////////////////////
void MolPolEMFieldSetup::UpdateFieldFZB4()
{

	fLocalFieldManagerFZB4->SetDetectorField(fMagFieldFZB4);

	if(fChordFinderFZB4) delete fChordFinderFZB4;
	fIntgrDriverFZB4 = new G4MagInt_Driver(fMinStep,fStepperFZB4,fStepperFZB4->GetNumberOfVariables());
	fChordFinderFZB4 = new G4ChordFinder((G4MagneticField*) fMagFieldFZB4, fMinStep, fStepperFZB4);
	fLocalFieldManagerFZB4->SetChordFinder( fChordFinderFZB4 );
	
}

void MolPolEMFieldSetup::UpdateFieldFZB5()
{

	fLocalFieldManagerFZB5->SetDetectorField(fMagFieldFZB5);

	if(fChordFinderFZB5) delete fChordFinderFZB5;
	fIntgrDriverFZB5 = new G4MagInt_Driver(fMinStep,fStepperFZB5,fStepperFZB5->GetNumberOfVariables());
	fChordFinderFZB5 = new G4ChordFinder((G4MagneticField*) fMagFieldFZB5, fMinStep, fStepperFZB5);
	fLocalFieldManagerFZB5->SetChordFinder( fChordFinderFZB5 );
	
}

/////////////////////////////////////////////////////////////////////////////
//
// Set stepper according to the stepper type
//


void MolPolEMFieldSetup::SetStepper()
{
	G4int nvar = 8;

	if(fStepper) delete fStepper;

	switch ( fStepperType )
	{
	case 0:
		fStepper = new G4ExplicitEuler( fEquation, nvar );
		break;
	case 1:
		fStepper = new G4ImplicitEuler( fEquation, nvar );
		break;
	case 2:
		fStepper = new G4SimpleRunge( fEquation, nvar );
		break;
	case 3:
		fStepper = new G4SimpleHeum( fEquation, nvar );
		break;
	case 4:
		fStepper = new G4ClassicalRK4( fEquation, nvar );
		break;
	case 5:
		fStepper = new G4CashKarpRKF45( fEquation, nvar );
		break;
	default: fStepper = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
