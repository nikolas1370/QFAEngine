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

/*-----*/

int QFAClassInfoBase::QfaClassCount = 0;
std::vector<QFAClassInfoBase*> QFAClassInfoBase::QCI;
QFAGameCodeFunctions QFAClassInfoBase::QFCFs;


extern "C" QFAGameCodeFunctions* QFAGetFunctions()
{
    QFAClassInfoBase::QFCFs.CreateObject = &QFAClassInfoBase::CreateObject;
    QFAClassInfoBase::QFCFs.DeleteObject = &QFAClassInfoBase::DeleteObject;
    QFAClassInfoBase::QFCFs.GetGameClassList = &QFAClassInfoBase::GetGameClassList;
    return &QFAClassInfoBase::QFCFs;
}
