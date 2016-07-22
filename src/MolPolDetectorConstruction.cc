#include "MolPolDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "MolPolDetector.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Para.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4OpticalSurface.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "MolPolEMFieldSetup.hh"

//#include <iostream>
//using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MolPolDetectorConstruction::DetModeSet(G4int detMode = 1) {
	
}

void MolPolDetectorConstruction::StandModeSet(G4int standMode = 0) {
        
}


MolPolDetectorConstruction::MolPolDetectorConstruction() {    
  mEMFieldSetup = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MolPolDetectorConstruction::~MolPolDetectorConstruction(){
	if(mEMFieldSetup) delete mEMFieldSetup;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* MolPolDetectorConstruction::Construct() {

  // Define materials
    
  G4double a, z, density, pressure, temperature;
  G4int nelements, natoms;

  
  // Air
  G4Element* N  = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);


  G4Element* Fe = new G4Element("Iron"   , "Fe", z=26, a=55.845*g/mole);
  G4Element* Si = new G4Element("Silicon", "Si", z=14, a=28.09 *g/mole);
  density = 7.65 *g/cm3;
  G4Material* siliconsteel = new G4Material("SiliconSteel", density, nelements=2);
  siliconsteel->AddElement(Fe, natoms=11);
  siliconsteel->AddElement(Si, natoms=1);


  //G4Element* Vac= new G4Element("Vacuum"  , "Vac", z=0 , a=06.00*g/mole);

  density = 1.e-6/760.0 * 1.29*mg/cm3; //0.001 of air density                                                                                                                                                
  pressure = 1.e-6/760.0 *atmosphere;
  temperature = 293.15 *kelvin;  //room temperature                                                                                                                                                          
  a = 28.97 *g/mole;

  G4Material* Vacuum = new G4Material("Vacuum",z=1,a,density,kStateGas,temperature,pressure);
  G4Material* Air    = new G4Material("Air",    density=1.29*mg/cm3, nelements=2);
  //G4Material* Vacuum = new G4Material("Vacuum", density=0.00*mg/cm3, nelements=1);
  Air->AddElement(N, 70.*perCent);
  Air->AddElement(O, 30.*perCent);
  //Vacuum->AddElement(Vac, 100.*perCent);

  G4double world_x, world_y, world_z;
  
  world_x = world_y = world_z = 10*m;

  G4VisAttributes* IronVisAtt = new G4VisAttributes(G4Colour(100./255.,149./255.,237./255.));
  G4VisAttributes* LeadVisAtt = new G4VisAttributes(G4Colour(149./255.,149./255.,100./255.));
  G4VisAttributes* SteelVisAtt= new G4VisAttributes(G4Colour(0./255.,0./255.,237./255.));
  G4VisAttributes* AlumVisAtt = new G4VisAttributes(G4Colour(0./255.,237./255.,0./255.));  
  G4VisAttributes* VacVisAtt = new G4VisAttributes(G4Colour(120./255.,237./255.,120./255.));  

  G4Box* world_box = new G4Box("World",world_x,world_y,world_z);
    
  G4LogicalVolume* world_log
    = new G4LogicalVolume(world_box,Vacuum,"World",0,0,0);
  
  world_log->SetVisAttributes(G4VisAttributes::Invisible);
  
  G4VPhysicalVolume* world_phys
    = new G4PVPlacement(0,G4ThreeVector(),world_log,"World",0,false,0);    

  mEMFieldSetup = new MolPolEMFieldSetup();  //setup the field, 
  G4FieldManager* Q1FieldManager = mEMFieldSetup->GetFieldManagerFZB1();
  G4FieldManager* Q2FieldManager = mEMFieldSetup->GetFieldManagerFZB2();
  G4FieldManager* Q3FieldManager = mEMFieldSetup->GetFieldManagerFZB3();
  G4FieldManager* Q4FieldManager = mEMFieldSetup->GetFieldManagerFZB4();
  G4FieldManager* DFieldManager  = mEMFieldSetup->GetFieldManagerFZB5();
  G4bool allLocal = true;



  /*
    GPARMED32   221 'Near vacuum,  mf$   '  21  0  1 80. -1.  5.   -1.   0.1    -1.
    GPARMED20   211 'Copper   mf$        '  11  0  1 80. -1. -1.   -1.   0.002  -1.
    GPARMED16   209 'Alum,  mf$          '   9  0  1 80. -1. -1.   -1.   0.2    -1.
    GPARMED17    10 'Steel nmf$          '  10  0  0  0. -1. -1.   -1.   0.2    -1.
    GPARMED31    21 'Near vacuum, nmf$   '  21  0  0  0. -1. -1.   -1.   0.1    -1.
    GPARMED23    13 'Lead  nmf$          '  13  0  0  0. -1. -1.   -1.   0.1    -1.
    GPARMED25    15 'Air,  nmf$          '  15  0  0  0. -1. -1.   -1.   0.1    -1.


    C --     Target chamber
    GPARVOL09  'TARG'   0  'HALL'    0.    0.    6.9  0  'TUBE'  3   0.     1.      1.
    GPARVOL10  'BPIT' 209  'HALL'    0.    0.    0.   0  'TUBE'  3   0.     5.08  100.
    GPARVOL11  'BPVT' 221  'BPIT'    0.    0.    0.   0  'TUBE'  3   0.     4.78  100.
    C --     Helmholtz coils warm
    C GPARVOL15  'HLMZ' 211  'TARG'    0.    0.   -5.8  0  'TUBE'  3   7.80  11.8     1.6
    C GVOLPOS01  'HLMZ'      'TARG'    0.    0.    5.8  0
    C --     Helmholtz coils cold
    GPARVOL15  'HLMZ' 211  'HALL'    0.    0.   -5.7  0  'TUBE'  3   5.10  15.0     5.
    GVOLPOS01  'HLMZ'      'HALL'    0.    0.   19.5  0
    C --     Target
    GPARVOL20  'MTAT' 270  'BPVT'    0.    0.    6.9  0  'TUBE'  3   0.    1.5    0.002
    C --     Beam pipe
    GPARVOL30  'BPI1' 209  'HALL'    0.    0.  200.   0  'TUBE'  3   0.     5.08  100.
    GPARVOL31  'BPV1' 221  'BPI1'    0.    0.    0.   0  'TUBE'  3   0.     4.78  100.
    GPARVOL32  'BPI2' 209  'HALL'    0.    0.  312.15 0  'TUBE'  3   0.     5.08  12.15
    GPARVOL33  'BPV2' 221  'BPI2'    0.    0.    0.   0  'TUBE'  3   0.     4.78  12.15
    GPARVOL34  'BPI3' 209  'HALL'    0.    0.  726.08 0  'TUBE'  3   0.     3.177 158.5 ***2.5"pipe
    GPARVOL35  'BPV3' 221  'BPI3'    0.    0.    0.   0  'TUBE'  3   0.     2.877 158.5 ***
    GPARVOL36  'BPF3'  10  'BPV3'    0.    0.  157.0  0  'TUBE'  3   1.905  4.78   1.5
    GPARVOL37  'BPF4'  10  'BPV3'    0.    0. -157.0  0  'TUBE'  3   3.000  4.78   1.5
    GPARVOL38  'BPI4'  10  'HALL'    0.    0. 1084.58 0  'TUBE'  3   0.     1.905 200.
    GPARVOL39  'BPV4'  21  'BPI4'    0.    0.    0.   0  'TUBE'  3   0.     1.800 200.
    GPARVOL40  'BPE4'  10  'HALL'    0.    0.  940.   0  'TUBE'  3   2.2    8.0    20.
    GPARVOL41  'BPI5' 209  'HALL'    0.    0.  545.08 0  'TUBE'  3   0.0    3.175  22.5
    GPARVOL42  'BPV5' 221  'BPI5'    0.    0.    0.   0  'TUBE'  3   0.0    3.000  22.5
  */

  /*
    C --  Syracuse hodoscope, SPACAL, auxill. artificial plane
    GPARVOL101 'HOD1'  15  'MDET'    0.    0.  -32.5  0  'BOX '  3   9.2  15.5     0.75
    GPARVOL102 'HOD2'   9  'MDET'    0.    0.  -16.0  0  'BOX '  3   9.2  16.5    15.2
    GPARVOL103 'HOD3'  15  'MDET'    0.    0.  -33.5  0  'BOX '  3   9.2  17.0     0.1
    C --     Shielding
    GPARVOL120 'S1LD'  13  'HALL'    0.  -29.  537.   0  'BOX '  3  30.  25. 14.0
    GPARVOL121 'S1H1'  15  'S1LD'   -4.1   9.5   0.   9  'PARA'  6  10.5 14. 1.6 10. 0. 0.
    GPARVOL122 'S1H2'  15  'S1LD'    4.1   9.5   0.   9  'PARA'  6  10.5 14. 1.6 10. 0. 0.
    GPARVOL123 'S2LD'  13  'HALL'    0.   -7.5 610.   0  'BOX '  3  17.   3.5 50.0 !!!***
    GPARVOL124 'S21B'   9  'HALL'    0.  -13.3 620.   0  'BOX '  3  31.   1.9  3.8 !!!***
    GPARVOL125 'S21H'  15  'S21B'    0.   -0.5   0.   0  'BOX '  3  31.   1.4  3.3
    GPARVOL126 'S3LD'  13  'HALL'    0.   -7.5 723.   0  'BOX '  3  20.   2.0 57.0  !!!***
  */

  // // // // // // // // DIPOLE BOX // // // // // // // //

  G4double buffer = 1.0 * cm;

  G4double pDBI1HLX =  6.0   * cm;  G4double pDBI1HLY = 16.5 * cm;  G4double pDBI1HLZ = 98.5  * cm;
  G4double pDBV1HLX =  5.295 * cm;  G4double pDBV1HLY = 16.0 * cm;  G4double pDBV1HLZ = 98.5  * cm + buffer;
  G4double pDBW1HLX = 11.66  * cm;  G4double pDBW1HLY = 21.4 * cm;  G4double pDBW1HLZ =  0.64 * cm;
  G4double pDBW2Rin =  0.0   * cm;  G4double pDBW2Rout=  3.0 * cm;  G4double pDBW2HL  =  0.64 * cm;
  G4double pDBW3HLX =  1.18  * cm;  G4double pDBW3HLY =  8.0 * cm;  G4double pDBW3HLZ =  0.64 * cm;
  G4double pDBW4HLX =  1.18  * cm;  G4double pDBW4HLY =  8.0 * cm;  G4double pDBW4HLZ =  0.64 * cm;
  G4double pDBT3HLX =  1.18  * cm;  G4double pDBT3HLY =  8.0 * cm;  G4double pDBT3HLZ =  0.005* cm;
  G4double pDBT4HLX =  1.18  * cm;  G4double pDBT4HLY =  8.0 * cm;  G4double pDBT4HLZ =  0.005* cm;
  G4double pDMS1HLX =  3.0   * cm;  G4double pDMS1HLY = 15.0 * cm;  G4double pDMS1HLZ = 96.2  * cm;
  G4double pDMBHRin =  0.0   * cm;  G4double pDMBHRout=  2.0 * cm;  G4double pDMBHHL  = 96.2  * cm + buffer;
  G4double pDMFRRin =  1.27  * cm;  G4double pDMFRRout=  2.0 * cm;  G4double pDMFRHL  =  9.0  * cm;

  G4double pDBI1Pos_X =  0.0   * cm;  G4double pDBI1Pos_Y = -9.0 * cm;  G4double pDBI1Pos_Z =422.8  * cm;
  G4double pDBV1Pos_X =  0.0   * cm;  G4double pDBV1Pos_Y =  0.0 * cm;  G4double pDBV1Pos_Z =  0.0  * cm;
  G4double pDBW1Pos_X =  0.0   * cm;  G4double pDBW1Pos_Y = -9.0 * cm;  G4double pDBW1Pos_Z =521.94 * cm;
  G4double pDBW2Pos_X =  0.0   * cm;  G4double pDBW2Pos_Y =  9.0 * cm;  G4double pDBW2Pos_Z =  0.0  * cm;
  G4double pDBW3Pos_X = -4.13  * cm;  G4double pDBW3Pos_Y = -7.0 * cm;  G4double pDBW3Pos_Z =  0.0  * cm;
  G4double pDBW4Pos_X =  4.13  * cm;  G4double pDBW4Pos_Y = -7.0 * cm;  G4double pDBW4Pos_Z =  0.0  * cm;
  G4double pDBT3Pos_X =  0.0   * cm;  G4double pDBT3Pos_Y =  0.0 * cm;  G4double pDBT3Pos_Z =  0.0  * cm;
  G4double pDBT4Pos_X =  0.0   * cm;  G4double pDBT4Pos_Y =  0.0 * cm;  G4double pDBT4Pos_Z =  0.0  * cm;
  G4double pDMS1Pos_X =  0.0   * cm;  G4double pDMS1Pos_Y =  0.0 * cm;  G4double pDMS1Pos_Z =  0.0  * cm;
  G4double pDMBHPos_X =  0.0   * cm;  G4double pDMBHPos_Y =  9.0 * cm;  G4double pDMBHPos_Z =  0.0  * cm;
  G4double pDMFRPos_X =  0.0   * cm;  G4double pDMFRPos_Y =  0.0 * cm;  G4double pDMFRPos_Z =-87.2  * cm;

  G4VSolid* DBI1Solid = new G4Box ( "DBI1Box" , pDBI1HLX , pDBI1HLY  , pDBI1HLZ );
  G4VSolid* DBV1Solid = new G4Box ( "DBV1Box" , pDBV1HLX , pDBV1HLY  , pDBV1HLZ );
  G4VSolid* DBW1Solid = new G4Box ( "DBW1Box" , pDBW1HLX , pDBW1HLY  , pDBW1HLZ );
  G4VSolid* DBW2Solid = new G4Tubs( "DBW2Tubs", pDBW2Rin , pDBW2Rout , pDBW2HL  , 0.0, 360.0 * deg );
  G4VSolid* DBW3Solid = new G4Box ( "DBW3Box" , pDBW3HLX , pDBW3HLY  , pDBW3HLZ );
  G4VSolid* DBW4Solid = new G4Box ( "DBW4Box" , pDBW4HLX , pDBW4HLY  , pDBW4HLZ );
  G4VSolid* DBT3Solid = new G4Box ( "DBT3Box" , pDBT3HLX , pDBT3HLY  , pDBT3HLZ );
  G4VSolid* DBT4Solid = new G4Box ( "DBT4Box" , pDBT4HLX , pDBT4HLY  , pDBT4HLZ );
  G4VSolid* DMS1Solid = new G4Box ( "DMS1Box" , pDMS1HLX , pDMS1HLY  , pDMS1HLZ );
  G4VSolid* DMBHSolid = new G4Tubs( "DMBHTubs", pDMBHRin , pDMBHRout , pDMBHHL  , 0.0, 360.0 * deg );
  G4VSolid* DMFRSolid = new G4Tubs( "DMFRTubs", pDMFRRin , pDMFRRout , pDMFRHL  , 0.0, 360.0 * deg );

  /*
  G4LogicalVolume* DBI1Logical = new G4LogicalVolume ( DBI1Solid, Air, "DBI1Logical", 0, 0, 0);
  G4LogicalVolume* DBV1Logical = new G4LogicalVolume ( DBV1Solid, Air, "DBV1Logical", 0, 0, 0);
  G4LogicalVolume* DBW1Logical = new G4LogicalVolume ( DBW1Solid, Air, "DBW1Logical", 0, 0, 0);
  G4LogicalVolume* DBW2Logical = new G4LogicalVolume ( DBW2Solid, Air, "DBW2Logical", 0, 0, 0);
  G4LogicalVolume* DBW3Logical = new G4LogicalVolume ( DBW3Solid, Air, "DBW3Logical", 0, 0, 0);
  G4LogicalVolume* DBW4Logical = new G4LogicalVolume ( DBW4Solid, Air, "DBW4Logical", 0, 0, 0);
  G4LogicalVolume* DBT3Logical = new G4LogicalVolume ( DBT3Solid, Air, "DBT3Logical", 0, 0, 0);
  G4LogicalVolume* DBT4Logical = new G4LogicalVolume ( DBT4Solid, Air, "DBT4Logical", 0, 0, 0);
  G4LogicalVolume* DMS1Logical = new G4LogicalVolume ( DMS1Solid, Air, "DMS1Logical", 0, 0, 0);
  G4LogicalVolume* DMBHLogical = new G4LogicalVolume ( DMBHSolid, Air, "DMBHLogical", 0, 0, 0);
  G4LogicalVolume* DMFRLogical = new G4LogicalVolume ( DMFRSolid, Air, "DMFRLogical", 0, 0, 0);
  */

  G4SubtractionSolid* sub1 = new G4SubtractionSolid("sub1", DBI1Solid, DBV1Solid, 0, G4ThreeVector(pDBV1Pos_X,
												   pDBV1Pos_Y,
												   pDBV1Pos_Z) );
  
  G4UnionSolid*       sub2 = new G4UnionSolid      ("sub2", sub1     , DMS1Solid, 0, G4ThreeVector(pDMS1Pos_X + pDBV1Pos_X,
												   pDMS1Pos_Y + pDBV1Pos_Y,
												   pDMS1Pos_Z + pDBV1Pos_Z) );
  
  G4SubtractionSolid* sub3 = new G4SubtractionSolid("sub3", sub2     , DMBHSolid, 0, G4ThreeVector(pDMBHPos_X + pDMS1Pos_X + pDBV1Pos_X,
												   pDMBHPos_Y + pDMS1Pos_Y + pDBV1Pos_Y,
												   pDMBHPos_Z + pDMS1Pos_Z + pDBV1Pos_Z) );
  //cout << pDMBHPos_X + pDMS1Pos_X + pDBV1Pos_X << " " << pDMBHPos_Y + pDMS1Pos_Y + pDBV1Pos_Y << " " << pDMBHPos_Z + pDMS1Pos_Z + pDBV1Pos_Z << endl; 
  G4UnionSolid*       sub4 = new G4UnionSolid      ("sub4", sub3     , DMFRSolid, 0, G4ThreeVector(pDMFRPos_X + pDMBHPos_X + pDMS1Pos_X + pDBV1Pos_X,
												   pDMFRPos_Y + pDMBHPos_Y + pDMS1Pos_Y + pDBV1Pos_Y,
												   pDMFRPos_Z + pDMBHPos_Z + pDMS1Pos_Z + pDBV1Pos_Z) );
  
  G4LogicalVolume* sub4Logical = new G4LogicalVolume ( sub4, Air, "sub4Logical", 0, 0, 0);

  G4SubtractionSolid* sub9 = new G4SubtractionSolid("sub9"  , DBW1Solid, DBW2Solid, 0, G4ThreeVector(pDBW2Pos_X, pDBW2Pos_Y, pDBW2Pos_Z) );
  G4SubtractionSolid* sub10= new G4SubtractionSolid("sub10" , sub9     , DBW3Solid, 0, G4ThreeVector(pDBW3Pos_X, pDBW3Pos_Y, pDBW3Pos_Z) );
  G4SubtractionSolid* sub11= new G4SubtractionSolid("sub11" , sub10    , DBW4Solid, 0, G4ThreeVector(pDBW4Pos_X, pDBW4Pos_Y, pDBW4Pos_Z) );

  G4LogicalVolume* sub11Logical = new G4LogicalVolume ( sub11, Air, "sub11Logical", 0, 0, 0);

  G4LogicalVolume* DLogical = new G4LogicalVolume ( DBI1Solid, Vacuum, "DLogical", 0, 0, 0);
  DLogical->SetVisAttributes(G4VisAttributes::Invisible);

  sub4Logical ->SetVisAttributes(LeadVisAtt);
  sub11Logical->SetVisAttributes(LeadVisAtt);

  //new G4PVPlacement(0,G4ThreeVector(pDBI1Pos_X, pDBI1Pos_Y, pDBI1Pos_Z),sub4Logical,"sub4",world_log,0,0,0);

  new G4PVPlacement(0,G4ThreeVector(pDBI1Pos_X, pDBI1Pos_Y, pDBI1Pos_Z),DLogical,"D",world_log,0,0,0);

  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),sub4Logical,"sub4",DLogical,0,0,0);
  
  //new G4PVPlacement(0,G4ThreeVector(pDBW1Pos_X, pDBW1Pos_Y, pDBW1Pos_Z),sub11Logical,"sub11",world_log,0,0,0);
  /*
    GPARVOL45 'DBI1' STE 'HALL'
    GPARVOL46 'DBV1' VAC 'DBI1'  
    GPARVOL53 'DMS1' STE 'DBV1'
    GPARVOL54 'DMBH' VAC 'DMS1' 
    GPARVOL55 'DMFR' STE 'DMBH'

    GPARVOL47 'DBW1' STE 'HALL'
    GPARVOL48 'DBW2' VAC 'DBW1'  
    GPARVOL49 'DBW3' AIR 'DBW1'  
    GPARVOL50 'DBW4' AIR 'DBW1'  
    GPARVOL51 'DBT3' TIT 'DBW3' 
    GPARVOL52 'DBT4' TIT 'DBW4' 

  */

  // // // // // // // // COLLIMATOR // // // // // // // //
  //GPARVOL60  'DCOL' 213  'DBV1'   -4.0   9.  -92.   0  'BOX '  3   1.    4.00    3.0
  //GPARVOL61  'DSLO' 221  'DCOL'    0.    0.    0.   0  'BOX '  3   1.    1.50    3.0
  //GVOLPOS02  'DCOL'      'DBV1'    4.0   9.  -92.   0
  
  G4double pDCOLHLX = 1.00   * cm;  G4double pDCOLHLY = 4.00 * cm;  G4double pDCOLHLZ = 3.00  * cm;
  G4double pDSLOHLX = 1.00   * cm;  G4double pDSLOHLY = 4.00 * cm;  G4double pDSLOHLZ = 3.00  * cm;
  G4double pDCOLPos_X =-4.00   * cm;  G4double pDCOLPos_Y = 9.00 * cm;  G4double pDCOLPos_Z =-92.00  * cm;
  G4double pDSLOPos_X = 0.00   * cm;  G4double pDSLOPos_Y = 0.00 * cm;  G4double pDSLOPos_Z =  0.00  * cm;
  
  G4VSolid* DCOLSolid = new G4Box ( "DCOLBox" , pDCOLHLX , pDCOLHLY  , pDCOLHLZ );
  G4VSolid* DSLOSolid = new G4Box ( "DSLOBox" , pDSLOHLX , pDSLOHLY  , pDSLOHLZ );

  G4SubtractionSolid* subcol = new G4SubtractionSolid("subcol", DCOLSolid, DSLOSolid, 0, G4ThreeVector(pDSLOPos_X,
												       pDSLOPos_Y,
												       pDSLOPos_Z) );

  G4LogicalVolume* subcolLogical = new G4LogicalVolume ( subcol, Air, "subcolLogical", 0, 0, 0);
  subcolLogical ->SetVisAttributes(IronVisAtt);
  new G4PVPlacement(0,G4ThreeVector( pDCOLPos_X + pDBV1Pos_X + pDBI1Pos_X, 
				     pDCOLPos_Y + pDBV1Pos_Y + pDBI1Pos_Y, 
				     pDCOLPos_Z + pDBV1Pos_Z + pDBI1Pos_Z),subcolLogical,"subcol",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(-pDCOLPos_X + pDBV1Pos_X + pDBI1Pos_X,
				    pDCOLPos_Y + pDBV1Pos_Y + pDBI1Pos_Y,
				    pDCOLPos_Z + pDBV1Pos_Z + pDBI1Pos_Z),subcolLogical,"subcol",world_log,0,0,0);

  // // // // // // // // MAGNETS // // // // // // // //

  G4double pQ1Rin  =  5.08 * cm;  G4double pQ1Rout = 20.00 * cm;  G4double pQ1HL   = 18.29 * cm;  G4double pQ1Pos_Z= 75.19 * cm;
  G4double pQ2Rin  =  5.08 * cm;  G4double pQ2Rout = 20.00 * cm;  G4double pQ2HL   = 22.30 * cm;  G4double pQ2Pos_Z=140.46 * cm;
  G4double pQ3Rin  =  5.08 * cm;  G4double pQ3Rout = 20.00 * cm;  G4double pQ3HL   = 18.37 * cm;  G4double pQ3Pos_Z=209.59 * cm;
  G4double pQ4Rin  =  5.08 * cm;  G4double pQ4Rout = 20.00 * cm;  G4double pQ4HL   = 18.37 * cm;  G4double pQ4Pos_Z=274.59 * cm;
  G4double pDHLX   = 10.00 * cm;  G4double pDHLY   = 40.00 * cm;  G4double pDHLZ   = 76.00 * cm;
  G4double pD1Pos_X=-16.00 * cm;  G4double pD2Pos_X= 16.00 * cm;  G4double pDPos_Y = -9.00 * cm;  G4double pDPos_Z =422.80 * cm;

  G4VSolid* Q1Solid = new G4Tubs( "Q1Tubs", pQ1Rin, pQ1Rout, pQ1HL, 0.0, 360.0 * deg );
  G4VSolid* Q2Solid = new G4Tubs( "Q2Tubs", pQ2Rin, pQ2Rout, pQ2HL, 0.0, 360.0 * deg );
  G4VSolid* Q3Solid = new G4Tubs( "Q3Tubs", pQ3Rin, pQ3Rout, pQ3HL, 0.0, 360.0 * deg );
  G4VSolid* Q4Solid = new G4Tubs( "Q4Tubs", pQ4Rin, pQ4Rout, pQ4HL, 0.0, 360.0 * deg );
  G4VSolid* Q1MagSolid = new G4Tubs( "Q1MagTubs", 0., pQ1Rin, pQ1HL, 0.0, 360.0 * deg );
  G4VSolid* Q2MagSolid = new G4Tubs( "Q2MagTubs", 0., pQ2Rin, pQ2HL, 0.0, 360.0 * deg );
  G4VSolid* Q3MagSolid = new G4Tubs( "Q3MagTubs", 0., pQ3Rin, pQ3HL, 0.0, 360.0 * deg );
  G4VSolid* Q4MagSolid = new G4Tubs( "Q4MAgTubs", 0., pQ4Rin, pQ4HL, 0.0, 360.0 * deg );
  //G4VSolid* DMagSolid  = new G4Box ( "DBox"  , pDHLX , pDHLY  , pDHLZ );
  G4LogicalVolume* Q1Logical = new G4LogicalVolume(Q1Solid,siliconsteel,"Q1Logical",0,0,0);
  G4LogicalVolume* Q2Logical = new G4LogicalVolume(Q2Solid,siliconsteel,"Q2Logical",0,0,0);
  G4LogicalVolume* Q3Logical = new G4LogicalVolume(Q3Solid,siliconsteel,"Q3Logical",0,0,0);
  G4LogicalVolume* Q4Logical = new G4LogicalVolume(Q4Solid,siliconsteel,"Q4Logical",0,0,0);
  G4LogicalVolume* Q1MagLogical = new G4LogicalVolume(Q1MagSolid,Vacuum,"Q1MagLogical",0,0,0);
  G4LogicalVolume* Q2MagLogical = new G4LogicalVolume(Q2MagSolid,Vacuum,"Q2MagLogical",0,0,0);
  G4LogicalVolume* Q3MagLogical = new G4LogicalVolume(Q3MagSolid,Vacuum,"Q3MagLogical",0,0,0);
  G4LogicalVolume* Q4MagLogical = new G4LogicalVolume(Q4MagSolid,Vacuum,"Q4MagLogical",0,0,0);
  //G4LogicalVolume* DMagLogical  = new G4LogicalVolume(DMagSolid ,Vacuum,"DMagLogical" ,0,0,0);
  Q1Logical->SetVisAttributes(IronVisAtt);
  Q2Logical->SetVisAttributes(IronVisAtt);
  Q3Logical->SetVisAttributes(IronVisAtt);
  Q4Logical->SetVisAttributes(IronVisAtt);

  Q1MagLogical->SetVisAttributes(VacVisAtt);
  Q1MagLogical->SetFieldManager(Q1FieldManager,allLocal);

  Q2MagLogical->SetVisAttributes(VacVisAtt);
  Q2MagLogical->SetFieldManager(Q2FieldManager,allLocal);

  Q3MagLogical->SetVisAttributes(VacVisAtt);
  Q3MagLogical->SetFieldManager(Q3FieldManager,allLocal);

  Q4MagLogical->SetVisAttributes(VacVisAtt);
  Q4MagLogical->SetFieldManager(Q4FieldManager,allLocal);

  //DMagLogical ->SetVisAttributes(IronVisAtt);
  DLogical->SetFieldManager(DFieldManager,allLocal);

  //G4VPhysicalVolume* Q1_phys =
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ1Pos_Z),Q1Logical,"Q1Phys",world_log,0,0,0);
  //G4VPhysicalVolume* Q2_phys = 
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ2Pos_Z),Q2Logical,"Q2Phys",world_log,0,0,0);
  //G4VPhysicalVolume* Q3_phys = 
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ3Pos_Z),Q3Logical,"Q3Phys",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ4Pos_Z),Q4Logical,"Q4Phys",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ1Pos_Z),Q1MagLogical,"Q1MagPhys",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ2Pos_Z),Q2MagLogical,"Q2MagPhys",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ3Pos_Z),Q3MagLogical,"Q3MagPhys",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ4Pos_Z),Q4MagLogical,"Q4MagPhys",world_log,0,0,0);

  //new G4PVPlacement(0,G4ThreeVector(pD1Pos_X,pDPos_Y,pDPos_Z),DMagLogical,"D1Phys",world_log,0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(pD2Pos_X,pDPos_Y,pDPos_Z),DMagLogical,"D2Phys",world_log,0,0,0);


  // // // // // // // // Virtual Boundary // // // // // // // //

  G4VSolid*        VBSolid   = new G4Tubs("VBSolid",0,pQ1Rout *5., 0.00001 * mm ,0.0,360.0*deg);
  G4LogicalVolume* Q1ENLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q1ENLogical",0,0,0);
  G4LogicalVolume* Q1EXLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q1EXLogical",0,0,0);
  G4LogicalVolume* Q2ENLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q2ENLogical",0,0,0);
  G4LogicalVolume* Q2EXLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q2EXLogical",0,0,0);
  G4LogicalVolume* Q3ENLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q3ENLogical",0,0,0);
  G4LogicalVolume* Q3EXLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q3EXLogical",0,0,0);
  G4LogicalVolume* Q4ENLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q4ENLogical",0,0,0);
  G4LogicalVolume* Q4EXLogical = new G4LogicalVolume(VBSolid, Vacuum, "Q4EXLogical",0,0,0);
  Q1ENLogical->SetVisAttributes(VacVisAtt);
  Q1EXLogical->SetVisAttributes(VacVisAtt);
  Q2ENLogical->SetVisAttributes(VacVisAtt);
  Q2EXLogical->SetVisAttributes(VacVisAtt);
  Q3ENLogical->SetVisAttributes(VacVisAtt);
  Q3EXLogical->SetVisAttributes(VacVisAtt);
  Q4ENLogical->SetVisAttributes(VacVisAtt);
  Q4EXLogical->SetVisAttributes(VacVisAtt);

  MolPolDetector* Q1ENSD = new MolPolDetector("q1en", 1);
  MolPolDetector* Q1EXSD = new MolPolDetector("q1ex", 2);
  MolPolDetector* Q2ENSD = new MolPolDetector("q2en", 3);
  MolPolDetector* Q2EXSD = new MolPolDetector("q2ex", 4);
  MolPolDetector* Q3ENSD = new MolPolDetector("q3en", 5);
  MolPolDetector* Q3EXSD = new MolPolDetector("q3ex", 6);
  MolPolDetector* Q4ENSD = new MolPolDetector("q4en", 7);
  MolPolDetector* Q4EXSD = new MolPolDetector("q4ex", 8);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  SDman->AddNewDetector(Q1ENSD);
  SDman->AddNewDetector(Q1EXSD);
  SDman->AddNewDetector(Q2ENSD);
  SDman->AddNewDetector(Q2EXSD);
  SDman->AddNewDetector(Q3ENSD);
  SDman->AddNewDetector(Q3EXSD);
  SDman->AddNewDetector(Q4ENSD);
  SDman->AddNewDetector(Q4EXSD);

  Q1ENLogical->SetSensitiveDetector(Q1ENSD);
  Q1EXLogical->SetSensitiveDetector(Q1EXSD);
  Q2ENLogical->SetSensitiveDetector(Q2ENSD);
  Q2EXLogical->SetSensitiveDetector(Q2EXSD);
  Q3ENLogical->SetSensitiveDetector(Q3ENSD);
  Q3EXLogical->SetSensitiveDetector(Q3EXSD);
  Q4ENLogical->SetSensitiveDetector(Q4ENSD);
  Q4EXLogical->SetSensitiveDetector(Q4EXSD);

  new G4PVPlacement(0,G4ThreeVector(0,0,pQ1Pos_Z - pQ1HL ),
		    Q1ENLogical,"virtualBoundaryPhys_q1en",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ1Pos_Z + pQ1HL ),
		    Q1EXLogical,"virtualBoundaryPhys_q1ex",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ2Pos_Z - pQ2HL ),
		    Q2ENLogical,"virtualBoundaryPhys_q2en",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ2Pos_Z + pQ2HL ),
		    Q2EXLogical,"virtualBoundaryPhys_q2ex",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ3Pos_Z - pQ3HL ),
		    Q3ENLogical,"virtualBoundaryPhys_q3en",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ3Pos_Z + pQ3HL ),
		    Q3EXLogical,"virtualBoundaryPhys_q3ex",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ4Pos_Z - pQ4HL ),
		    Q4ENLogical,"virtualBoundaryPhys_q4en",world_log,0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0,0,pQ4Pos_Z + pQ4HL ),
		    Q4EXLogical,"virtualBoundaryPhys_q4ex",world_log,0,0,0);



  // // // // // // // // DETECTOR AND BOX // // // // // // // //

  G4double pMDBXHLX   = 37.00 * cm;  G4double pMDBXHLY   = 47.10 * cm;  G4double pMDBXHLZ   = 61.20 * cm;
  G4double pMDBAHLX   = 18.50 * cm;  G4double pMDBAHLY   = 29.80 * cm;  G4double pMDBAHLZ   = 40.00 * cm;
  G4double pMDBWHLX   = 13.70 * cm;  G4double pMDBWHLY   = 11.50 * cm;  G4double pMDBWHLZ   =  6.45 * cm;
  G4double pMDBWalpha =  6.0  *deg;  G4double pMDBWtheta =  0.0  *deg;  G4double pMDBWphi   =  0.0  *deg;
  G4double pMDBLHLX   = 13.70 * cm;  G4double pMDBLHLY   = 11.50 * cm;  G4double pMDBLHLZ   =  6.45 * cm;
  G4double pMDBLalpha = 10.0  *deg;  G4double pMDBLtheta =  0.0  *deg;  G4double pMDBLphi   =  0.0  *deg;
  G4double pMDETHLX   =  9.20 * cm;  G4double pMDETHLY   = 17.00 * cm;  G4double pMDETHLZ   = 34.00 * cm;
  G4double pDLGBHLX   =  8.20 * cm;  G4double pDLGBHLY   = 16.20 * cm;  G4double pDLGBHLZ   = 20.10 * cm;

  G4double pMDBXPos_X   =  0.00 * cm;  G4double pMDBXPos_Y   =-50.10 * cm;  G4double pMDBXPos_Z   =724.00 * cm;
  G4double pMDBAPos_X   =  0.00 * cm;  G4double pMDBAPos_Y   =  1.10 * cm;  G4double pMDBAPos_Z   =  1.80 * cm;
  G4double pMDBWPos_X   =  0.00 * cm;  G4double pMDBWPos_Y   =  8.50 * cm;  G4double pMDBWPos_Z   =-49.70 * cm;
  G4double pMDBLPos_X   =  0.00 * cm;  G4double pMDBLPos_Y   =  7.50 * cm;  G4double pMDBLPos_Z   =-49.70 * cm;
  G4double pMDETPos_X   =  0.00 * cm;  G4double pMDETPos_Y   =  0.00 * cm;  G4double pMDETPos_Z   =  0.00 * cm;
  G4double pDLGBPos_X   =  0.00 * cm;  G4double pDLGBPos_Y   =  0.00 * cm;  G4double pDLGBPos_Z   =-11.00 * cm;

  //Rotations
  //RMATR07  90.   0.  80.  90.  10.  270.     Moller detector
  //RMATR09  90. 270.   0.   0.  90.  180.     I is oppos Y,II along Z,III oppos X
  G4RotationMatrix* pRot9 = new G4RotationMatrix();
  pRot9->rotateZ(-90.*deg);
  pRot9->rotateY(-90.*deg);
  G4RotationMatrix* pRot7 = new G4RotationMatrix();
  pRot7->rotateX(-10.*deg);

  G4VSolid* MDBXSolid  = new G4Box ( "MDBXBox"  , pMDBXHLX, pMDBXHLY, pMDBXHLZ );
  G4VSolid* MDBASolid  = new G4Box ( "MDBABox"  , pMDBAHLX, pMDBAHLY, pMDBAHLZ );
  G4VSolid* MDBWSolid  = new G4Para( "MDBWPara" , pMDBWHLX, pMDBWHLY, pMDBWHLZ, pMDBWalpha, pMDBWtheta, pMDBWphi);
  G4VSolid* MDBLSolid  = new G4Para( "MDBLPara" , pMDBLHLX, pMDBLHLY, pMDBLHLZ, pMDBLalpha, pMDBLtheta, pMDBLphi);
  G4VSolid* MDETSolid  = new G4Box ( "MDETBox"  , pMDETHLX, pMDETHLY, pMDETHLZ );
  G4VSolid* DLGBSolid  = new G4Box ( "DLGBBox"  , pDLGBHLX, pDLGBHLY, pDLGBHLZ );

  G4SubtractionSolid* sub5 = new G4SubtractionSolid("sub5", MDBXSolid, MDBASolid, 0    , G4ThreeVector(pMDBAPos_X, pMDBAPos_Y, pMDBAPos_Z) );
  G4SubtractionSolid* sub6 = new G4SubtractionSolid("sub6", sub5     , MDBWSolid, pRot9, G4ThreeVector(pMDBWPos_X, pMDBWPos_Y, pMDBWPos_Z) );
  G4SubtractionSolid* sub7 = new G4SubtractionSolid("sub7", sub6     , MDBLSolid, pRot9, G4ThreeVector(pMDBLPos_X, pMDBLPos_Y, pMDBLPos_Z) );
  G4SubtractionSolid* sub8 = new G4SubtractionSolid("sub8", sub7     , MDETSolid, pRot7, G4ThreeVector(pMDETPos_X, pMDETPos_Y, pMDETPos_Z) );

  G4LogicalVolume* sub8Logical = new G4LogicalVolume ( sub8, Air, "sub8Logical", 0, 0, 0);
  sub8Logical->SetVisAttributes(AlumVisAtt);

  new G4PVPlacement(0,G4ThreeVector(pMDBXPos_X, pMDBXPos_Y, pMDBXPos_Z),sub8Logical,"sub8",world_log,0,0,0);


  /*
    C --     Detector box
    GPARVOL80  'MDBX'  13  'HALL'    0.  -50.1 724.   0  'BOX '  3  37.   47.1   61.2  ***7cm up
    GPARVOL81  'MDBA'  15  'MDBX'    0.    1.1   1.8  0  'BOX '  3  18.5  29.8   40.0
    GPARVOL82  'MDBW'  15  'MDBX'    0.    8.5 -49.7  9  'PARA'  6  13.7  11.5   6.45 6.0 0. 0.
    GPARVOL83  'MDBL'  15  'MDBX'    0.    7.5 -49.7  9  'PARA'  6  13.7  11.5   6.45 10.0 0. 0.
    C --     Detector
    GPARVOL90  'MDET'  15  'MDBA'    0.    0.    0.   7  'BOX '  3   9.2  17.0    34.0
    GPARVOL91  'DLGB'   9  'MDET'    0.    0.  -11.   0  'BOX '  3   8.2  16.2    20.1
  */

  
  
  return world_phys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    
    
    
    


