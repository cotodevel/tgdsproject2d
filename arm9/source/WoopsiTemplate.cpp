// Includes
#include "typedefsTGDS.h"
#include "WoopsiTemplate.h"
#include "woopsiheaders.h"
#include "bitmapwrapper.h"
#include "bitmap.h"
#include "graphics.h"
#include "rect.h"
#include "gadgetstyle.h"
#include "fonts/newtopaz.h"
#include "woopsistring.h"
#include "colourpicker.h"
#include "filerequester.h"
#include "soundTGDS.h"
#include "main.h"
#include "posixHandleTGDS.h"
#include "keypadTGDS.h"
#include "gui_console_connector.h"
#include "TGDSLogoLZSSCompressed.h"
#include "videoTGDS.h"
#include "math.h"
#include "imagepcx.h"
#include "dswnifi_lib.h"
#include "timerTGDS.h"
#include "powerTGDS.h"
#include "debugNocash.h"
#include "loader.h"

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void enableWaitForVblankC(){
	WoopsiTemplateProc->enableWaitForVblank();
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void disableWaitForVblankC(){
	WoopsiTemplateProc->disableWaitForVblank();
}

__attribute__((section(".dtcm")))
WoopsiTemplate * WoopsiTemplateProc = NULL;

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::startup(int argc, char **argv){

	//tama setup
	//render TGDSLogo from a LZSS compressed file
	RenderTGDSLogoMainEngine((uint8*)&TGDSLogoLZSSCompressed[0], TGDSLogoLZSSCompressed_size);
	
	initSound();
	enableWaitForVblankC();
	Rect rect;

	/** SuperBitmap preparation **/
	// Create bitmap for superbitmap
	Bitmap* superBitmapBitmap = new Bitmap(164, 191);

	// Get a graphics object from the bitmap so that we can modify it
	Graphics* gfx = superBitmapBitmap->newGraphics();

	// Clean up
	delete gfx;

	// Create screens
	uiScreen = new AmigaScreen(TGDSPROJECTNAME, Gadget::GADGET_DECORATION, AmigaScreen::AMIGA_SCREEN_SHOW_DEPTH);
	woopsiApplication->addGadget(uiScreen);
	uiScreen->setPermeable(true);

	// Add child windows
	AmigaWindow* controlWindow = new AmigaWindow(0, 13, 256, 33, "Controls", Gadget::GADGET_DECORATION, AmigaWindow::AMIGA_WINDOW_SHOW_DEPTH);
	uiScreen->addGadget(controlWindow);

	// Controls
	controlWindow->getClientRect(rect);

	_Index = new Button(rect.x, rect.y, 41, 16, "<");	
	_Index->setRefcon(1);
	controlWindow->addGadget(_Index);
	_Index->addGadgetEventHandler(this);
	
	_lastFile = new Button(rect.x + 41, rect.y, 41, 16, "v");
	_lastFile->setRefcon(2);
	controlWindow->addGadget(_lastFile);
	_lastFile->addGadgetEventHandler(this);
	
	_nextFile = new Button(rect.x + 41 + 41, rect.y, 41, 16, ">");
	_nextFile->setRefcon(3);
	controlWindow->addGadget(_nextFile);
	_nextFile->addGadgetEventHandler(this);
	
	_play = new Button(rect.x + 41 + 41 + 41, rect.y, 80, 16, "Run TGDS-MB");
	_play->setRefcon(4);
	controlWindow->addGadget(_play);
	_play->addGadgetEventHandler(this);
	
	_MultiLineTextBoxLogger = NULL;	//destroyable TextBox
	
	enableDrawing();	// Ensure Woopsi can now draw itself
	redraw();			// Draw initial state
	
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::shutdown()   {
	Woopsi::shutdown();
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::waitForAOrTouchScreenButtonMessage(MultiLineTextBox* thisLineTextBox, const WoopsiString& thisText)   {
	thisLineTextBox->appendText(thisText);
	scanKeys();
	while((!(keysDown() & KEY_A)) && (!(keysDown() & KEY_TOUCH))){
		scanKeys();
	}
	scanKeys();
	while((keysDown() & KEY_A) && (keysDown() & KEY_TOUCH)){
		scanKeys();
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleValueChangeEvent(const GadgetEventArgs& e)   {

	// Did a gadget fire this event?
	if (e.getSource() != NULL) {
		/* 
		//Destroyable Textbox implementation init
		Rect rect;
		uiScreen->getClientRect(rect);
		_MultiLineTextBoxLogger = new MultiLineTextBox(rect.x, rect.y, 262, 170, "Loading\n...", Gadget::GADGET_DRAGGABLE, 5);
		uiScreen->addGadget(_MultiLineTextBoxLogger);
		
		_MultiLineTextBoxLogger->removeText(0);
		_MultiLineTextBoxLogger->moveCursorToPosition(0);
		_MultiLineTextBoxLogger->appendText("File open OK: ");
		_MultiLineTextBoxLogger->appendText(strObj);
		_MultiLineTextBoxLogger->appendText("\n");
		_MultiLineTextBoxLogger->appendText("Please wait calculating CRC32... \n");
		
		char arrBuild[256+1];
		sprintf(arrBuild, "%s%x\n", "Invalid file: crc32 = 0x", crc32);
		_MultiLineTextBoxLogger->appendText(WoopsiString(arrBuild));
		
		sprintf(arrBuild, "%s%x\n", "Expected: crc32 = 0x", 0x5F35977E);
		_MultiLineTextBoxLogger->appendText(WoopsiString(arrBuild));
		
		waitForAOrTouchScreenButtonMessage(_MultiLineTextBoxLogger, "Press (A) or tap touchscreen to continue. \n");
		
		_MultiLineTextBoxLogger->invalidateVisibleRectCache();
		uiScreen->eraseGadget(_MultiLineTextBoxLogger);
		_MultiLineTextBoxLogger->destroy();	//same as delete _MultiLineTextBoxLogger;
		//Destroyable Textbox implementation end
		*/
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleLidClosed() {
	// Lid has just been closed
	_lidClosed = true;

	// Run lid closed on all gadgets
	s32 i = 0;
	while (i < _gadgets.size()) {
		_gadgets[i]->lidClose();
		i++;
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleLidOpen() {
	// Lid has just been opened
	_lidClosed = false;

	// Run lid opened on all gadgets
	s32 i = 0;
	while (i < _gadgets.size()) {
		_gadgets[i]->lidOpen();
		i++;
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void printMessage(char * msg){
	//Destroyable Textbox implementation init
	Rect rect;
	WoopsiTemplateProc->uiScreen->getClientRect(rect);
	WoopsiTemplateProc->_MultiLineTextBoxLogger = new MultiLineTextBox(rect.x, rect.y, 262, 170, "Loading\n...", Gadget::GADGET_DRAGGABLE, 5);
	WoopsiTemplateProc->uiScreen->addGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
	
	WoopsiTemplateProc->_MultiLineTextBoxLogger->removeText(0);
	WoopsiTemplateProc->_MultiLineTextBoxLogger->moveCursorToPosition(0);
	WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText(":");
	WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText("\n");
	
	WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText(WoopsiString(msg));
	
	WoopsiTemplateProc->waitForAOrTouchScreenButtonMessage(WoopsiTemplateProc->_MultiLineTextBoxLogger, "Press (A) or tap touchscreen to continue. \n");
	
	WoopsiTemplateProc->_MultiLineTextBoxLogger->invalidateVisibleRectCache();
	WoopsiTemplateProc->uiScreen->eraseGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
	WoopsiTemplateProc->_MultiLineTextBoxLogger->destroy();	//same as delete _MultiLineTextBoxLogger;
	//Destroyable Textbox implementation end
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleClickEvent(const GadgetEventArgs& e)   {
	switch (e.getSource()->getRefcon()) {
		//Left Button Event
		case 1:{
			
		}	
		break;
		
		//Middle Button Event
		case 2:{
			
		}	
		break;
		
		//Right Button Event
		case 3:{
			
		}	
		break;
		
		//Exit Button Event
		case 4:{
			//Destroyable Textbox implementation init
			Rect rect;
			WoopsiTemplateProc->uiScreen->getClientRect(rect);
			WoopsiTemplateProc->_MultiLineTextBoxLogger = new MultiLineTextBox(rect.x, rect.y, 262, 170, "Loading\n...", Gadget::GADGET_DRAGGABLE, 5);
			WoopsiTemplateProc->uiScreen->addGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
			
			WoopsiTemplateProc->_MultiLineTextBoxLogger->removeText(0);
			WoopsiTemplateProc->_MultiLineTextBoxLogger->moveCursorToPosition(0);
			WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText(":");
			WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText("\n");
			
			WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText("Are you sure you want to run TGDS-Multiboot?\n(A) No\n(B) Yes");
			bool pressedB = false;
			while(1==1){
				scanKeys();
				if(keysDown() & KEY_A){
					break;
				}
				if(keysDown() & KEY_B){
					pressedB = true;
					break;
				}
			}
			WoopsiTemplateProc->_MultiLineTextBoxLogger->invalidateVisibleRectCache();
			WoopsiTemplateProc->uiScreen->eraseGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
			WoopsiTemplateProc->_MultiLineTextBoxLogger->destroy();	//same as delete _MultiLineTextBoxLogger;
			//Destroyable Textbox implementation end
			
			
			if(pressedB == true){
				
				char * TGDS_CHAINLOADEXEC = NULL;
				if(__dsimode == true){
					TGDS_CHAINLOADEXEC = "0:/ToolchainGenericDS-multiboot.srl";
				}
				else{
					TGDS_CHAINLOADEXEC = "0:/ToolchainGenericDS-multiboot.nds";
				}
				char thisArgv[4][MAX_TGDSFILENAME_LENGTH];
				memset(thisArgv, 0, sizeof(thisArgv));
				strcpy(&thisArgv[0][0], "");	//Arg0:	This Binary loaded
				strcpy(&thisArgv[1][0], "");	//Arg1:	NDS Binary to chainload through TGDS-MB
				strcpy(&thisArgv[2][0], "");	//Arg2: NDS Binary loaded from TGDS-MB	
				u32 * payload = getTGDSMBV3ARM7Bootloader();
				
				bool isTGDSTWLHomebrew = false;
				if(isNTROrTWLBinary(TGDS_CHAINLOADEXEC, &isTGDSTWLHomebrew) != notTWLOrNTRBinary){				
					if(TGDSMultibootRunNDSPayload(TGDS_CHAINLOADEXEC, (u8*)payload, 0, (char*)&thisArgv) == false){ //should never reach here, nor even return true. Should fail it returns false
						while(1==1){
							bool waitForVblank = false;
							int threadsRan = runThreads(internalTGDSThreads, waitForVblank);			
						}
					}
				}
			}
		}	
		break;
		
		
	}
}

__attribute__((section(".dtcm")))
u32 pendPlay = 0;

char currentFileChosen[256+1];

//Called once Woopsi events are ended: TGDS Main Loop
__attribute__((section(".itcm")))
void Woopsi::ApplicationMainLoop()  {
	//Earlier.. main from Woopsi SDK.
	
	//Handle TGDS stuff...

	//Timeout screens
	u32 pressed = keysDown();
	if(
		(pressed&KEY_TOUCH)
		||
		(pressed&KEY_A)
		||
		(pressed&KEY_B)
		||
		(pressed&KEY_UP)
		||
		(pressed&KEY_DOWN)
		||
		(pressed&KEY_LEFT)
		||
		(pressed&KEY_RIGHT)
		||
		(pressed&KEY_L)
		||
		(pressed&KEY_R)
		){
		bottomScreenIsLit = true; //input event triggered
		triggerSpecialEffect = false;
		iconCallbackWakeup = true;
		iconCallbackWakeupAcknowledgeTimeout = 17;
		millisecondsElapsed = 0;
		
		if(pressed & KEY_R){	
			GUI.GBAMacroMode = !GUI.GBAMacroMode; //swap LCD
			TGDSLCDSwap();
			scanKeys();
			while(keysDown() & KEY_R){
				scanKeys();
			}
		}

		if(GUI.GBAMacroMode == true){
			setBacklight(POWMAN_BACKLIGHT_BOTTOM_BIT);
		}
		else{
			setBacklight(POWMAN_BACKLIGHT_TOP_BIT|POWMAN_BACKLIGHT_BOTTOM_BIT);
		}
	}

	bool waitForVblank = false;
	int threadsRan = runThreads(internalTGDSThreads, waitForVblank);
}