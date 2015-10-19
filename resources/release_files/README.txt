
Four Entry points to choose from: Two versions of each entry point, SysNAND and EmuNAND options are available in one version now!

==========================================================================================
FOR ALL VERSIONS: COPY THE D9UI FOLDER TO THE ROOT OF YOUR SD CARD OR THE MENU WON'T WORK!
==========================================================================================


1. Ninjhax Version (new and old 3ds)
===================================
	Copy the Decrypt9UI folder to your 3ds sd card "3ds" folder so ninjhax homebrew menu can see it and run it from the homebrew menu.

2. Brahma/Brahma2 Version (new and old 3ds)
===================================
	Raw bin file for running from the Brahma Homebrew app, similar to above but copy the Decrypt9UI.bin to the /brahma folder on the sd card root. Then load your Brahma Homebrew app and select the Decrypt9UI.bin from the list and run it.

3. Spider (browser) Version (old 3ds only)
==========================================
	Copy the Launcher.dat to the sd card root. Then run it from go.gateway-3ds.com (or some other browser loader, dukesrg's one for example.)

4. NEW! MSET Version (new and old 3ds)
======================================
	Copy the Decrypt9UI.dat to the sd card root. Also copy the Decrypt9UIROP.nds to your sd card and run it (from sudokuhax or ds flashcart), then select the option that matches your system settings (MSET) version, for most people it will be 4.x DG (old 3ds) or 4.x NDG (new 3ds) then press A and A again and exit. The load it from System Settings -> Other Settings -> Profile -> Nintendo DS Profile.

======================================

====================
SD CARD PREPARATION
====================

If you haven't created it first and want to save yourself from rebooting again, create a folder on the sd root called Decrypt9.

The following files belong / live / are generated in there:

	*	slot0x25KeyX.bin
	*	ncchinfo.bin
	*	seeddb.bin
	*	sdinfo.bin
	*	decTitleKeys.bin
	*	encTitleKeys.bin
	*	decTitleKeys_emu.bin
	*	encTitleKeys_emu.bin
	*	ticket.db
	*	ticket_emu.db
	
	*	And possibly something else I'm forgetting right now...
	
	*	Xorpads when generated will go here.
	*	NAND Backups when generated will go here.
	*	NAND Partition Backups when generated will go here.
	
	*	EVERYTHING WILL GO HERE!
	
Also while in this folder create another folder called D9titles.

	*	Games/Roms etc you want to decrypt will go in here.

======================================

=========
Scripts:
=========

	*	Python scripts of note here are as follows:
	
		*	ncchinfo_gen.py			* Used to generate ncchinfo.bin to generate xorpads for games.
		*	ncchinfo_tgen.py		* Used to generate ncchinfo.bin to generate xorpads for theme files.
		*	ncchinfo_gen-exh.py		* Used to generate ncchinfo.bin to generate exefs and exheader xorpads for games. Useful for converting games to cia with Riku's Simple Cia Converter.
		

	*	Python scripts not really noteworthy but still useful at times:
	
		*	dump_ticket_keys.py		* Used to generate encTitlekeys.bin from ticket.db. - Not really needed as this is built in to Decrypt9 now.
		*	print_ticket_keys.py	* Used to print decrypted titlekeys from decTitleKeys.bin once it is generated from the previous file (or directly from the option in Decrypt9). Example usage: print_ticket_keys.py decTitleKeys.bin > keys.txt
		*	sdinfo_gen.py			* Used for the SD Padgen option to generate SDinfo.bin which then generates xorpads for already installed content on your SD card.
		*	seeddb_gen.py			* Used to generate seeddb.bin from the seed file stored in nand:\\data<console-unique>\\sysdata\\0001000f\\ on 9.6+ emunand on old 3ds only (emunand up to 9.5 only available on n3ds at present) - Not really needed now.
		*	cdn_download.py			* Used to download content from nintendo's servers. Example usage: cdn_download.py TitleID TitleKey

======================================

=======
MENUS:
=======

These all have small explanations in the actual menu but I'll put some here too. :)


	*	Decryption
	
		*	NCCH PADGEN				* Used to generate xorpads for games, themes and other stuff that uses ncchinfo.bin.
		*	SD PADGEN				* Used to generate xorpads for games, themes and other stuff that uses SDinfo.bin.
		*	ROM DECRYPTION			* Used to directly decrypt games on the 3ds which can then be extracted etc on PC with ctrtool.
		*	TITLEKEYS FILE			* Not much to say here, it generates decTitleKeys.bin from encTitleKeys.bin.
		*	TITLEKEYS NAND			* Not much to say here, it generates decTitleKeys.bin from the titlekeys stored in your SYSNAND.
		*	DUMP TICKET DB			* Dumps your ticket.db from SYSNAND. Useful for using it with Funkycia and other tools.
		*	NAND FAT16 XORPAD		* Generates the SYSNAND fat16 partition xorpad.
		*	TWL FAT16 XORPAD		* Generates the SYSNAND TWL fat16 partition xorpad.
		*	FIRM0 XORPAD			* Generates the SYSNAND firm0 and firm1 partitions xorpad.
		
	
	*	NAND OPTIONS
	
		*	NAND BACKUP				* Creates a backup of your whole SYSNAND in its current state.
		*	NAND RESTORE			* Restores the backup created in the previous option.
		*	DUMP PARTITIONS			* Dumps and decrypts the CTR NAND fat16 partition from SYSNAND.
		*	RESTORE PARTITIONS		* Restores the partition backup created in the previous option.
		*	TWL BACKUP				* Dumps and decrypts the TWLN fat16 partition from SYSNAND.
		*	TWL RESTORE				* Restores the partition backup created in the previous option.
		*	DUMP MOVEABLE.SED		* Dumps moveable.sed from nand/private/ in SYSNAND. Useful for manual system transfers.
		*	DUMP SECUREINFO_A		* Dumps SecureInfo_A from nandrw/sys/ in SYSNAND. Useful for region changing.
		*	INJECT MOVEABLE.SED		* Injects moveable.sed into SYSNAND.
		*	INJECT SECUREINFO_A		* Injects SecureInfo_A into SYSNAND.
	
	
	*	EMUNAND
	
		*	DUMP TICKET DB			* Dumps your ticket.db from EMUNAND. Useful for using it with Funkycia and other tools.
		*	DUMP MOVEABLE.SED		* Dumps moveable.sed from nand/private/ in EMUNAND. Useful for manual system transfers.
		*	DUMP SECUREINFO_A		* Dumps SecureInfo_A from nandrw/sys/ in EMUNAND. Useful for region changing.
		*	INJECT MOVEABLE.SED		* Injects moveable.sed into EMUNAND.
		*	INJECT SECUREINFO_A		* Injects SecureInfo_A into EMUNAND.
		*	DUMP SEEDSAVE.BIN		* Dumps seedsave.bin from nand:\\data<console-unique>\\sysdata\\0001000f\\ in EMUNAND. 9.6+ emunand on old 3ds only (emunand up to 9.5 only available on n3ds at present).
		*	UPDATE SEED DB			* Searches the EMUNAND for the seedsave and checks for new seeds and adds them to your seeddb.bin.
		*	TITLEKEYS NAND			* Not much to say here, it generates decTitleKeys_emu.bin from the titlekeys stored in your EMUNAND.

======================================

I think that has everything covered for now.

Any questions or issues go here: 

https://gbatemp.net/threads/download-decrypt9-wip-3dsx-launcher-dat.388831/

And hopefully me (Shadowtrance), or d0k3 can help. :)

Enjoy! :)
