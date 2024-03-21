#pragma once
enum QFAEditorFileTypes : short
{
	EFTUndefined = 0,
	EFTMesh = 1,
	EFTImage = 2
};

struct QFAEditorFileOnDisk
{
	const char signature[10] = "QFAEditor";
	QFAEditorFileTypes type;
	unsigned short version;
	QFAEditorFileOnDisk() {}
	QFAEditorFileOnDisk(QFAEditorFileTypes fileType, unsigned int fileVersion)
	{
		type = fileType;
		version = fileVersion;
	}
};