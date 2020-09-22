; ===== Date Created: 07 September, 2020 ===== 

%ifndef BIOS_PARAMETER_BLOCK_INL_ASM
%define BIOS_PARAMETER_BLOCK_INL_ASM
	
biosParameterBlock:
	; BIOS parameter block
	OEMName					db "PandaOS "
	bytesPerSector			dw 512
	sectorsPerCluster		db 1
	reservedSectors			dw 2
	FATCount				db 2
	rootDirectoryEntries	dw 224
	sectorsCount			dw 2880
	mediaDescriptor			db 0xf0
	sectorsPerFAT			dw 9
	sectorsPerTrack			dw 63
	headsCount				dw 2
	hiddenSectors			dd 0

	; Extended BIOS parameter block
	totalSectorsCount		dd 0
	bootDriveNumber			db 0
	reserved				db 0
	bootSignature			db 0x29
	volumeID				dd 0
	volumeLabel				db "PandaVolume"
	filesystemType			db "FAT16   "

%endif
