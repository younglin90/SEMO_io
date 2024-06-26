#pragma once

#include "TecGUIMenuActionAbstract.h"
#include "TecGUIGlobal.h"
#include "tecgui_Exports.h"
#include "ClassMacros.h"

namespace tecplot { namespace tecgui {


class TecGUIMenuAction
    : public TecGUIMenuActionAbstract
{
    Q_OBJECT

public:
    TecGUIMenuAction(
        GUIInt_t              parent,
        std::string const&              label,
        TecGUIVoidCallback_pf 	callback);

    virtual ~TecGUIMenuAction();

private slots:
    void menuActionTriggered(bool isChecked);

private:
    TecGUIVoidCallback_pf  m_callback;
    UNCOPYABLE_CLASS(TecGUIMenuAction);
};



}}
