#include "Object.h"

QObject::QObject()
{
    
}

QObject::~QObject()
{
    Valid = 0;
}



void QObject::Destroy()
{
    if (!IsValid())
        return;
    
    delete this;
}

int QFAClass::QfaClassCount = 0;
std::vector<QFAClass*> QFAClass::QCI;
QFAGameCodeFunctions QFAClass::QFCFs;

extern "C" QFAGameCodeFunctions* QFAGetFunctions()
{
    QFAClass::QFCFs.CreateObject = &QFAClass::CreateObject;
    QFAClass::QFCFs.DeleteObject = &QFAClass::DeleteObject;
    QFAClass::QFCFs.GetGameClassList = &QFAClass::GetGameClassList;
    return &QFAClass::QFCFs;
}