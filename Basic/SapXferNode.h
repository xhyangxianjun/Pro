#ifndef _SAPXFERNODE_H_
#define _SAPXFERNODE_H_

//	SapXferNode.h : header file
//

#include "SapClassBasicDef.h"

//
// SapXferParams class declaration
//
class SAPCLASSBASIC_CLASS SapXferParams
{
public:
   // Transfer frame types
   typedef int FrameType;
   enum _FrameType
   {
      FrameInterlaced  = CORXFER_VAL_FRAME_INTERLACED,
      FrameProgressive = CORXFER_VAL_FRAME_NON_INTERLACED
   };

   // Transfer field order
   typedef int FieldOrder;
   enum _FieldOrder
   {
      FieldOrderOddEven = CORXFER_VAL_FIELD_ORDER_ODD_EVEN,
      FieldOrderEvenOdd = CORXFER_VAL_FIELD_ORDER_EVEN_ODD,
      FieldOrderNext    = CORXFER_VAL_FIELD_ORDER_NEXT_FIELD
   };

public:
	// Constructor/destructor
	SapXferParams();
	~SapXferParams();

	// Access to implementation
   FrameType GetFrameType()       const { return m_Frame; }
   FieldOrder GetFieldOrder() const { return m_FieldOrder; }
	int GetWidth()        const { return m_Width; }
	int GetHeight()       const { return m_Height; }
	SapFormat GetFormat() const { return m_Format; }
	int GetPixelDepth()   const { return m_PixelDepth; }
	int GetPixelShift()   const { return m_PixelShift; }
	void GetParameters(FrameType *frame, FieldOrder *fieldOrder, int *width, int *height, SapFormat *format, int *pixelDepth, int *pixelShift) const;

	void SetFrameType(FrameType frameType) { m_Frame = frameType; }
	void SetFieldOrder(FieldOrder fieldOrder) { m_FieldOrder = fieldOrder; }
	void SetWidth(int width) { m_Width = width; }
	void SetHeight(int height) { m_Height = height; }
	void SetFormat(SapFormat format) { m_Format = format; SetPixelDepth(CorManGetPixelDepthMax(m_Format)); }
	void SetPixelDepth(int pixelDepth) { m_PixelDepth = pixelDepth; SetPixelShift(CorManGetPixelDepthMax(m_Format) - pixelDepth); }
	void SetPixelShift(int pixelShift) { m_PixelShift = pixelShift; }
	void SetParameters(FrameType frame, FieldOrder fieldOrder, int width, int height, SapFormat format, int pixelDepth, int pixelShift);

	// Utility
	operator CORXFER_DESC();

   // Obsolete methods
   FrameType GetFrame() const { return GetFrameType(); }
	void Get(FrameType *frame, FieldOrder *fieldOrder, int *width, int *height, SapFormat *format, int *pixelDepth, int *pixelShift) const;
	void SetFrame(FrameType frame) { SetFrameType(frame); }
	void Set(FrameType frame, FieldOrder fieldOrder, int width, int height, SapFormat format, int pixelDepth, int pixelShift);

protected:
	FrameType m_Frame;
	FieldOrder m_FieldOrder;
	int m_Width;
	int m_Height;
	SapFormat m_Format;
	int m_PixelDepth;
	int m_PixelShift;
};


//
// SapBufferParams class declaration
//
class SAPCLASSBASIC_CLASS SapBufferParams
{
public:
	// Constructor/destructor
   SapBufferParams();
   ~SapBufferParams();

   // Access to implementation
   double GetScan3dCoordScaleA() const {return m_Scan3dCoordScaleA;}
   double GetScan3dCoordScaleB() const {return m_Scan3dCoordScaleB;}
   double GetScan3dCoordScaleC() const {return m_Scan3dCoordScaleC;}
   double GetScan3dCoordOffsetA() const {return m_Scan3dCoordOffsetA;}
   double GetScan3dCoordOffsetB() const {return m_Scan3dCoordOffsetB;}
   double GetScan3dCoordOffsetC() const {return m_Scan3dCoordOffsetC;}
   BOOL GetScan3dInvalidDataFlagC() const {return m_Scan3dInvalidDataFlagC;}
   double GetScan3dInvalidDataValueC() const {return m_Scan3dInvalidDataValueC;}
   UINT32 GetScan3dDistanceUnit() const {return m_Scan3dDistanceUnit;}
   UINT32 GetDeviceScanType() const {return m_DeviceScanType;}
   UINT32 GetScan3dOutputMode() const {return m_Scan3dOutputMode;}

   void SetScan3dCoordScaleA(double scan3dCoordScale) {m_Scan3dCoordScaleA = scan3dCoordScale;}
   void SetScan3dCoordScaleB(double scan3dCoordScale) {m_Scan3dCoordScaleB = scan3dCoordScale;}
   void SetScan3dCoordScaleC(double scan3dCoordScale) {m_Scan3dCoordScaleC = scan3dCoordScale;}
   void SetScan3dCoordOffsetA(double scan3dCoordOffset) {m_Scan3dCoordOffsetA = scan3dCoordOffset;}
   void SetScan3dCoordOffsetB(double scan3dCoordOffset) {m_Scan3dCoordOffsetB = scan3dCoordOffset;}
   void SetScan3dCoordOffsetC(double scan3dCoordOffset) {m_Scan3dCoordOffsetC = scan3dCoordOffset;}
   void SetScan3dInvalidDataFlagC(BOOL scan3dInvalidDataFlag) {m_Scan3dInvalidDataFlagC = scan3dInvalidDataFlag;}
   void SetScan3dInvalidDataValueC(double scan3dInvalidDataValue) {m_Scan3dInvalidDataValueC = scan3dInvalidDataValue;}
   void SetScan3dDistanceUnit(UINT32 scan3dDistanceUnit) {m_Scan3dDistanceUnit = scan3dDistanceUnit;}
   void SetDeviceScanType(UINT32 deviceScanType) {m_DeviceScanType = deviceScanType;}
   void SetScan3dOutputMode(UINT32 scan3dOutputMode) {m_Scan3dOutputMode = scan3dOutputMode;}

protected:
   double m_Scan3dCoordScaleA;
   double m_Scan3dCoordScaleB;
   double m_Scan3dCoordScaleC;
   double m_Scan3dCoordOffsetA;
   double m_Scan3dCoordOffsetB;
   double m_Scan3dCoordOffsetC;
   BOOL m_Scan3dInvalidDataFlagC;
   double m_Scan3dInvalidDataValueC;
   UINT32 m_Scan3dDistanceUnit;
   UINT32 m_DeviceScanType;
   UINT32 m_Scan3dOutputMode;
};


//
// SapXferNode class declaration
//
class SAPCLASSBASIC_CLASS SapXferNode : public SapManager
{
public:
   // Possible types for derived classes
   enum XferNodeType
   {
      NodeTypeUnknown,
      NodeTypeAcqDevice,
      NodeTypeAcquisition,
      NodeTypeBuffer,
      NodeTypeCab,
      NodeTypeObsolete1,
      NodeTypePixPro,
      NodeTypeRTPro,
      NodeTypeBufferMulti
   };

   // Various constants
   enum MiscValues
   {
      MaxXferParams = 32
   };

public:
	// Constructor/Destructor
	SapXferNode(SapLocation location) 
		{ m_Location = location; m_pSrcNode = NULL; m_pBufferParamsNode = NULL; m_SrcPort = 0; m_hServer = m_handle = CORHANDLE_NULL; }

	SapXferNode(SapLocation location, SapXferNode *pSrcNode) 
		{ m_Location = location; m_pSrcNode = pSrcNode; m_pBufferParamsNode = NULL; m_SrcPort = 0; m_hServer = m_handle = CORHANDLE_NULL; }

	SapXferNode(SapLocation location, SapXferParams xferParams) 
		{ m_Location = location; m_pSrcNode = NULL; m_pBufferParamsNode = NULL; m_SrcPort = 0; m_XferParams[0] = xferParams; m_hServer = m_handle = CORHANDLE_NULL; }

   //should copy all xferParams
	SapXferNode(const SapXferNode& node) 
		{ m_Location = node.m_Location; m_pSrcNode = node.m_pSrcNode; m_pBufferParamsNode = node.m_pBufferParamsNode; m_SrcPort = node.m_SrcPort;m_XferParams[0] = node.m_XferParams[0]; m_hServer = m_handle = CORHANDLE_NULL; }

	virtual ~SapXferNode() {}

	SapXferNode& operator= (const SapXferNode& node) 
	{ 
		m_Location = node.m_Location; m_pSrcNode = node.m_pSrcNode; m_pBufferParamsNode = node.m_pBufferParamsNode; m_SrcPort = node.m_SrcPort; m_XferParams[0] = node.m_XferParams[0]; m_bInitOK = FALSE; m_hServer = m_handle = CORHANDLE_NULL; 
		return *this;
	}

	// Creation/destruction
	virtual BOOL Create() = 0;
	virtual BOOL Destroy() = 0;

public:
	// Access to implementation
	SapLocation GetLocation()		const { return m_Location; }
	SapXferNode *GetSrcNode()		const { return m_pSrcNode; }
   int GetSrcPort()              const { return m_SrcPort; }
   CORSERVER GetServer()         const { return m_hServer; }
	virtual SapXferParams GetXferParams(int portIndex = 0)	const { return m_XferParams[CORXFER_PORT_INDEX_MASK(portIndex)]; }
	virtual SapXferParams GetInputXferParams(int portIndex = 0)	const { return GetXferParams(CORXFER_PORT_INDEX_MASK(portIndex)); }
	virtual SapXferParams GetOutputXferParams(int portIndex = 0)	const { return GetXferParams(CORXFER_PORT_INDEX_MASK(portIndex)); }
	SapBufferParams* GetBufferParams()  { return m_pBufferParamsNode ? m_pBufferParamsNode->GetBufferParams() : &m_BufferParams; }

	BOOL SetLocation(SapLocation loc)					{ if( m_bInitOK) return FALSE; m_Location = loc; return TRUE;}
	virtual BOOL SetSrcNode(SapXferNode *pSrcNode, int srcPort = 0)				{ if( m_bInitOK) return FALSE; m_pSrcNode = pSrcNode; m_SrcPort = srcPort; return TRUE;}
	virtual BOOL SetBufferParamsNode(SapXferNode *pBufferParamsNode)				{ if( m_bInitOK) return FALSE; m_pBufferParamsNode = pBufferParamsNode; return TRUE;}
	virtual BOOL SetXferParams(SapXferParams xferParams, int portIndex = 0)		{ if( m_bInitOK) return FALSE; m_XferParams[portIndex] = xferParams; return TRUE;}
	virtual BOOL SetInputXferParams(SapXferParams params, int portIndex = 0)		{ return SetXferParams(params, CORXFER_PORT_INDEX_MASK(portIndex)); }
	virtual BOOL SetOutputXferParams(SapXferParams params, int portIndex = 0)	{ return SetXferParams(params, CORXFER_PORT_INDEX_MASK(portIndex)); }

	virtual CORHANDLE GetHandle()	const { return m_handle; }
	virtual BOOL UpdateXferParamsInSrcNode()
		{ return m_pSrcNode ? m_pSrcNode->UpdateXferParams() : TRUE; }
   virtual BOOL UpdateXferParams() { return TRUE; }
	virtual void GetXferParamsFromSrcNode() 
		{ if (m_pSrcNode) SetInputXferParams(m_pSrcNode->GetOutputXferParams( m_SrcPort)); }

   // All classes derived from SapXferNode need to implement this
   virtual XferNodeType GetXferNodeType() = 0;

   // Obsolete method
   virtual BOOL IsBuffer() { return GetXferNodeType() == NodeTypeBuffer; }

public:
	// Cycling specific methods
	virtual CORHANDLE GetHandle(int index)	   const { return index == 0 ? m_handle : CORHANDLE_NULL; }
	virtual CORHANDLE operator[] (int index)	const { return GetHandle(index); }
	virtual int GetCount() const { return 1; }
	virtual int GetIndex() const { return 0; }
	virtual BOOL SetIndex(int index) { return TRUE; }
	virtual void ResetIndex() {}
	virtual void Next() {}

	virtual CORHANDLE GetTrash() const { return CORHANDLE_NULL; }

   // Obsolete methods
   virtual BOOL Empty()          { return TRUE; }
   virtual BOOL Empty(int index) { return TRUE; }
   virtual BOOL EmptyAll()       { return TRUE; }

protected:
	// Parameters
	SapLocation		m_Location;
	SapXferNode		*m_pSrcNode;
	SapXferNode		*m_pBufferParamsNode;
   int            m_SrcPort;
	SapXferParams	m_XferParams[MaxXferParams];
   SapBufferParams m_BufferParams;
	CORSERVER		m_hServer;
	CORHANDLE		m_handle;
}; 

#endif	// _SAPXFERNODE_H_

