; ===== Date Created: 07 September, 2020 ===== 

%ifndef BIOS_PARAMETER_BLOCK_INL_ASM
%define BIOS_PARAMETER_BLOCK_INL_ASM
	
bios_parameter_block:
	; BIOS parameter block
	OEMName					db "PandaOS "
	BytesPerSector			dw 512
	SectorsPerCluster		db 1
	ReservedSectors			dw 1
	FATCount				db 2
	RootDirectoryEntries	dw 224
	SectorsCount			dw 2880
	MediaDescriptor			db 0xf0
	SectorsPerFAT			dw 9
	SectorsPerTrack			dw 9
	HeadsCount				dw 2
	HiddenSectors			dd 0

	; Extended BIOS parameter block
	TotalSectorsCount		dd 0
	BootDriveNumber			db 0
	Reserved				db 0
	BootSignature			db 0x29
	VolumeID				dd 0
	VolumeLabel				db "PandaVolume"
	FilesystemType			db "FAT16   "

%endif
