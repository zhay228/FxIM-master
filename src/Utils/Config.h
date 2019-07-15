#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "Utils/BaseDef.h"
#include "ThirdParty/tinyxml/tinyxml.h"

#define		KEY_REBUILD			0x0001
#define		KEY_DIAG			0x0002

// ����ѡ��
#define		CONFIG_ALL			0xFFFFFFFF
#define		CONFIG_CRYPT		0x00000001
#define		CONFIG_GENERAL		0x00000002
#define		CONFIG_NETWORK		0x00000004
#define		CONFIG_PERSONAL		0x00000008
#define		CONFIG_SKIN			0x00000010


#define		RS_REALTIME			0x00000001

struct Config
{
public:
	BOOL		ReadConfig();
	BOOL		WriteConfig(int nFlag = CONFIG_ALL);

public:
	//////////////////////�����������ر���START/////////////////////////////
	HCRYPTPROV	hCsp;
	HCRYPTPROV	hSmallCsp;
	PubKey		pubKey;
	PubKey		smallPubKey;	
	HCRYPTKEY	hPrivKey;
	HCRYPTKEY	hSmallPrivKey;

	int			privBlobLen;
	int			privEncryptType;
	int			privEncryptSeedLen;
	BYTE		*privBlob;	
	BYTE		*privEncryptSeed;
	//////////////////////��������������ϢEND/////////////////////////////

	// �������
	PIP_ADAPTER_INFO	pIpAdapterInfo;
	PIP_ADAPTER_INFO	pIpAdapterInfoUse;
	//BYTE				AddressUse[MAX_ADAPTER_ADDRESS_LENGTH];			// ����mac��ַ����PIP_ADAPTER_INFO.Address�����ڶ�д����ʱ��Ҫת�����Ƚϲ��㣬������Щ�����ĵ�ַ���ܶ���00-00-00-00-00�����߱�Ψһ��
	char				szAdapterNameUse[MAX_PATH];						// �������֣���PIP_ADAPTER_INFO.AdapterName  MAX_ADAPTER_NAME_LENGTH + 4 = MAX_PATH
	ULONG				nAddr;											// ʹ�õ�IP��ַ
	USHORT				nPort;

	
	TList				dialUpList;
	TBroadcastList		broadcastList;
	THosts				hosts;
	THosts				fileHosts;
	int					nResolveOpt;
	int					nDelayTime;
	BOOL				bExtendEntry;
	BOOL				bDialUpCheck;
	BOOL				bAbsenceCheck;
	int					nAbsenceMax;
	int					nAbsenceChoice;
	int					TransMax;						//�ļ����仺������С
	int					ViewMax;						//�ļ�����ʱ�ļ�ӳ���С
	int					fileTransOpt;					//�ļ����䷽ʽ

	TCHAR				(*szAbsenceStr)[MAX_PATH];
	TCHAR				(*szAbsenceHead)[MAX_NAMEBUF];

	TCHAR				szLastOpenDir[MAX_PATH];
	TCHAR				szPersonalDir[MAX_PATH];		// 
	TCHAR				szDefaultFileSaveDir[MAX_PATH]; // �ļ�Ĭ�Ͻ���Ŀ¼
	TCHAR				szChatImageSaveDir[MAX_PATH];	// ����ͼƬĿ¼
	TCHAR				szAvatarSaveDir[MAX_PATH];		// ͷ��Ŀ¼

	TCHAR				szNickName[MAX_NAMEBUF];
	TCHAR				szGroupName[MAX_NAMEBUF];
	TCHAR				szAvatar[MAX_PATH];				// File path

	TCHAR				szBgImage[MAX_PATH];			// ����ͼ������ɫ
	DWORD				dwBgColor;

	CharFormatLite		cfLite;							// ����������������
	BOOL				bWantReturnSend;				// �Ƿ�Enter��������Ϣ
	BOOL				bSendCheckOpt;					// ������Ϣ���Է����Ƿ���Ҫ�Է����ͻ�ִ��

	int					nWriteFlag;

	
private:
	TCHAR				szAppDir[MAX_PATH];
	TCHAR				szCfgFilePath[MAX_PATH];
private:
	void	SetElementValue(const char* elementName, const char* szValue,
				TiXmlElement* parentElement);
	void	SetElementValue(const char* elementName, unsigned long nValue, 
				TiXmlElement* parentElement);
	BOOL	GetElementValue(const char* elementName, char* szValue, int nMaxCount, 
				const TiXmlElement* parentElement);	
	BOOL	GetElementValue(const char* elementName, unsigned long* pnValue, 
				const TiXmlElement* parentElement);

	void	ReadCrypt(TiXmlElement* rootElement);
	void	WriteCrypt(TiXmlElement* rootElement);

	void	ReadPersonal(TiXmlElement* rootElement);
	void	WritePersonal(TiXmlElement* rootElement);

	void	ReadNetwork(TiXmlElement* rootElement);
	void	WriteNetwork(TiXmlElement* rootElement);

	void	ReadSkin(TiXmlElement* rootElement);
	void	WriteSkin(TiXmlElement* rootElement);
private:
	Config();
	~Config();

	void	Init();
	BOOL	GetAdapters();
	void	MatchAdapter();
	BOOL	IsByteNull(BYTE* b, int len);

	DECLARE_SINGLETON_CLASS(Config)
};

#endif //_CONFIG_H_