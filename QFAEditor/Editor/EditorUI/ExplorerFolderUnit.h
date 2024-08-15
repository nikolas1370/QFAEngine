#pragma once
#include <UI/UIParentHiddenChild.h>

class QFAEditorMainWindow;
class QFAText;
class QFAUIEditorFileExplorer;
class QFAEditorOptionWindow;
class QFAEditorExplorerFolderUnit : public QFAParentHiddenChild
{
	friend QFAEditorMainWindow;
	friend QFAUIEditorFileExplorer;
	friend QFAEditorOptionWindow;

	static QFAImage* FolderImage;
	static QFAImage* FileImage;
	QFAUICanvas* Canvas;
	QFAUIImage* Icon;	
	QFAText* UnitText;

protected:
	void MySlotChange(QFAUIUnit* unit) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void ChangeImage(bool foldar);
	void ChangeText(std::u32string text);

	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAEditorExplorerFolderUnit();
	~QFAEditorExplorerFolderUnit();
};

