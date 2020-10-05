; ===== Date Created: 07 September, 2020 ===== 

%ifndef BIOS_PARAMETER_BLOCK_INL_ASM
%define BIOS_PARAMETER_BLOCK_INL_ASM
	
biosParameterBlock:
	; BIOS parameter block
	OEMName					db "PandaOS "
	bytesPerSector			dw 512
	sectorsPerCluster		db 1
	reservedSectors			dw 3
	FATCount				db 2
	rootDirectoryEntries	dw 224
	sectorsCount			dw 2880
	mediaDescriptor			db 0xf8
	oldSectorsPerFAT		dw 0
	sectorsPerTrack			dw 63
	headsCount				dw 2
	hiddenSectors			dd 0

	; Extended BIOS parameter block
	; totalSectorsCount		dd 0
	; bootDriveNumber		db 0
	; reserved				db 0
	; bootSignature			db 0x29
	; volumeID				dd 0
	; volumeLabel			db "PandaVolume"
	; filesystemType		db "FAT16   "

	totalSectorsCount		dd 0
	sectorsPerFAT			dw 9
	driveDescription		dw 0
	version					dw 0x0100 ; Version 1
	rootDirectoryStart		dd 2
	fsInformationSector		dw 2
	bootBackupSector		dw 0
	reserved0				times 12 db 0
	bootDriveNumber			db 0
	reserved1				db 0
	bootSignature			db 0x29
	volumeId				dd 0
	volumeLabel				db "PandaVolume"
	filesystemType			db "FAT32   "

%endif
