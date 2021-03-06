//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file KernelAnalyzerEditorFactory.h
///
//==================================================================================

// KernelAnalyzerEditorFactory.h

#pragma once


// Local:
#include <src/vspKernelAnalyzerEditorDocument.h>
/***************************************************************************
CodeXLVSPackage.pkgdef contains:


[$RootKey$\Editors\{34918D20-6DE3-4DFB-B888-D5BDA6ABD775}]
@="CodeXLVSPackage"
"Package"="{ecdfbaee-ad99-452d-874c-99fce5a48b8e}"
"DisplayName"="CodeXLVSPackage"
"ExcludeDefTextEditor"=dword:00000001
"AcceptBinaryFiles"=dword:00000000

[$RootKey$\Editors\{34918D20-6DE3-4DFB-B888-D5BDA6ABD775}\LogicalViews]
"{7651A703-06E5-11D1-8EBD-00A0C90F26EA}"=""

[$RootKey$\Editors\{34918D20-6DE3-4DFB-B888-D5BDA6ABD775}\Extensions]
"myext"=dword:00000032

which informs the shell that CodeXLVSPackagePackage is the package to createto have this editor
registered with Visual Studio.  When CodeXLVSPackagePackage is sited by Visual Studio,
CodeXLVSPackagePackage::PostSited creates an instance of KernelAnalyzerEditorFactory and then queries
for SID_SVsRegisterEditors and then calls IVsRegisterEditors->RegisterEditor to register
the new instance of KernelAnalyzerEditorFactory with Visual Studio.

Visual Studio then uses the KernelAnalyzerEditorFactory instance to create EditorDocument
instances, which is the actual edtior instance.
***************************************************************************/

class KernelAnalyzerEditorFactory :
// Use ATL to take care of common COM infrastructure
    public CComObjectRootEx<CComSingleThreadModel>,
// IVsKernelAnalyzerEditorFactory is required to be an editor factory
    public IVsEditorFactoryImpl<KernelAnalyzerEditorFactory>
{

    // Provides a portion of the implementation of IUnknown, in particular the list of interfaces
    // the KernelAnalyzerEditorFactory object will support via QueryInterface
    BEGIN_COM_MAP(KernelAnalyzerEditorFactory)
    COM_INTERFACE_ENTRY(IVsEditorFactory)
    END_COM_MAP()

    // COM objects typically should not be cloned, and this prevents cloning by declaring the
    // copy constructor and assignment operator private (NOTE:  this macro includes the decleration of
    // a private section, so everything following this macro and preceding a public or protected
    // section will be private).
    VSL_DECLARE_NOT_COPYABLE(KernelAnalyzerEditorFactory)

protected:
    KernelAnalyzerEditorFactory()
    {
    }

    virtual ~KernelAnalyzerEditorFactory()
    {
    }


public:

#pragma warning(push)
#pragma warning(disable : 4480) // // warning C4480: nonstandard extension used: specifying underlying type for enum
    enum PhysicalViewId : unsigned int
    {
        Unsupported,
        Primary
    };
#pragma warning(pop)

    PhysicalViewId GetPhysicalViewId(REFGUID rguidLogicalView)
    {
        if (LOGVIEWID_Primary == rguidLogicalView || LOGVIEWID_TextView == rguidLogicalView)
        {
            return Primary;
        }

        return Unsupported;
    }

    BSTR GetPhysicalViewBSTR(PhysicalViewId viewId)
    {
        if (Primary == viewId)
        {
            // Note that the value of LOGVIEWID_TextView in the registry is empty string.
            // The values under LogicalViews for an editor should match up to the values
            // returned by this method, so that GetPhysicalViewId below can properly
            // map the strings back to a PhysicalViewId.
            return ::SysAllocString(L"");
        }

        return NULL;
    }

    PhysicalViewId GetPhysicalViewId(LPCOLESTR szPhysicalView)
    {
        if (szPhysicalView != NULL && szPhysicalView[0] == L'\0')
        {
            return Primary;
        }

        return Unsupported;
    }

    bool CanShareBuffer(PhysicalViewId /*physicalViewId*/)
    {
        return false;
    }

    void CreateSingleViewObject(
        PhysicalViewId physicalViewId,
        CComPtr<IUnknown>& rspViewObject,
        CComBSTR& rbstrEditorCaption,
        const GUID*& rpguidCommandUI,
        VSEDITORCREATEDOCWIN& /*rCreateDocumentWindowUI*/)
    {
        if (physicalViewId == Primary)
        {
            CComObject<vspKernelAnalyzerEditorDocument>* pDocument;
            VSL_CHECKHRESULT(CComObject<vspKernelAnalyzerEditorDocument>::CreateInstance(&pDocument));
            HRESULT hr = pDocument->QueryInterface(&rspViewObject);

            if (FAILED(hr))
            {
                // If QueryInterface failed, then there is something wrong with the object.
                // Delete it and throw an exception for the error.
                delete pDocument;
                VSL_CREATE_ERROR_HRESULT(hr);
            }

            // NOTE - if the file is read only [Read Only] will be appended to the caption
            rbstrEditorCaption.LoadStringW(IDS_EDITORCAPTION);

            rpguidCommandUI = &CLSID_CodeXLVSPackageKernelAnalyzerEditorDocument;

            // pCreateDocumentWindowUI will be initialized to 0, and see we provide
            // no GUI for the user to cancel from so leave it 0

            return;
        }

        ERRHR(E_FAIL); // This should never happen
    }

private:

};
