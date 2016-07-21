#ifndef MolPolMessenger_HH
#define MolPolMessenger_HH

#include "globals.hh"
#include "MolPoltypes.hh"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4VModularPhysicsList.hh"

/*!
 *   Global messenger class
 */

class MolPolIO;
class MolPolDetectorConstruction;
class MolPolEventAction;
class MolPolPrimaryGeneratorAction;
class MolPolSteppingAction;

class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIdirectory;

class MolPolMessenger : public G4UImessenger {
    public:
       	MolPolMessenger();
       	~MolPolMessenger();

	void SetIO( MolPolIO *io ){ fIO = io; }
	void SetPriGen( MolPolPrimaryGeneratorAction *pg ){ fprigen = pg; }
	void SetDetCon( MolPolDetectorConstruction *dc ){ fdetcon= dc; }
	void SetEvAct( MolPolEventAction *ev ){ fevact = ev; }
	void SetStepAct( MolPolSteppingAction *st ){ fStepAct = st; }

	void SetNewValue(G4UIcommand* cmd, G4String newValue);

    private:
	MolPolIO *fIO;
	MolPolDetectorConstruction *fdetcon;
	MolPolEventAction *fevact;
	MolPolPrimaryGeneratorAction *fprigen;
	MolPolSteppingAction *fStepAct;

	G4UIdirectory *fRemollDir;
	
	G4UIcmdWithAnInteger *seedCmd;
	G4UIcmdWithAString   *fileCmd;
	//
	//G4UIcmdWithAnInteger *fStandModeCmd;
	G4UIcmdWithAnInteger *fDetModeCmd;
	G4UIcmdWithAnInteger *fStandModeCmd;
	G4UIcmdWithAnInteger *fSourceModeCmd;
	G4UIcmdWithADouble *fQuartzPolishCmd;
	G4UIcmdWithADoubleAndUnit *fDetAngleCmd;
	// POSSCAN
	G4UIcmdWithADoubleAndUnit *fDetPosXCmd;	
	G4UIcmdWithADoubleAndUnit *fDetPosYCmd;	
	//
	G4UIcmdWithADoubleAndUnit *fXminCmd;
	G4UIcmdWithADoubleAndUnit *fXmaxCmd;
	G4UIcmdWithADoubleAndUnit *fYminCmd;
	G4UIcmdWithADoubleAndUnit *fYmaxCmd;
	G4UIcmdWithADoubleAndUnit *fEminCmd;
	G4UIcmdWithADoubleAndUnit *fEmaxCmd;

	G4UIcmdWithADoubleAndUnit *fthetaMinCmd;
	G4UIcmdWithADoubleAndUnit *fthetaMaxCmd;
	G4UIcmdWithADoubleAndUnit *fphiMinCmd;
	G4UIcmdWithADoubleAndUnit *fphiMaxCmd;

//	G4UIcmdWithADoubleAndUnit *fThetaCmd;
//	G4UIcmdWithADoubleAndUnit *fPhiCmd;
	G4UIcmdWithADoubleAndUnit *fZCmd;

};

#endif//MolPolMessenger_HH


