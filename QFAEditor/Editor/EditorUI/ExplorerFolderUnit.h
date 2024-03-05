#pragma once
#include <Render/UI/UIParentHiddenChild.h>

class QFAEditorMainWindow;
class QFAText;
class QFAUIEditorFileExplorer;
class QFAEditorExplorerFolderUnit : public QFAParentHiddenChild
{
	friend QFAEditorMainWindow;
	friend QFAUIEditorFileExplorer;

	static QFAImage* FolderImage;
	static QFAImage* FileImage;
	QFAUICanvas* Canvas;
	QFAUIImage* Icon;	
	QFAText* UnitText;

protected:
	void MySlotChange(QFAUIUnit* unit) override;
	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void ChangeImage(bool foldar);
	void ChangeText(std::u32string text);
public:
	QFAEditorExplorerFolderUnit();
	~QFAEditorExplorerFolderUnit();

private:


};

