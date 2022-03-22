#include "stdafx.h"
#include "MBaseChannelRule.h"
#include "MXml.h"
#include "MZFileSystem.h"



void MChannelRuleMapList::Add(string strMapName)
{
	for (int i = 0; i < MMATCH_MAP_MAX; i++)
	{
		if (!stricmp(strMapName.c_str(), g_MapDesc[i].szMapName))
		{
			Add(i);
			return;
		}
	}
}

bool MChannelRuleMapList::Exist(int nMapID, bool bOnlyDuel)
{
	set<int>::iterator itor = m_Set.find( nMapID);

	if ( itor != m_Set.end())
	{
		int id = (*itor);

		if ( !bOnlyDuel && g_MapDesc[id].bOnlyDuelMap)
			return false;

		return true;
	}

	return false;
}


bool MChannelRuleMapList::Exist(const char* pszMapName, bool bOnlyDuel)
{
	for (set<int>::iterator itor = m_Set.begin(); itor != m_Set.end(); ++itor)
	{
		int id = (*itor);

		if ((id >= 0) && (id < MMATCH_MAP_MAX))
		{
			if ( !stricmp(pszMapName, g_MapDesc[id].szMapName))
			{
				if ( !bOnlyDuel && g_MapDesc[id].bOnlyDuelMap)
					return false;

				return true;
			}
		}
	}

	return false;
}

#define MTOK_CHANNELRULE				"CHANNELRULE"
#define MTOK_CHANNELMAP					"MAP"
#define MTOK_CHANNELRULE_GAMETYPE		"GAMETYPE"
#define MTOK_CHANNELRULE_ATTR_ID		"id"
#define MTOK_CHANNELRULE_ATTR_NAME		"name"


MChannelRuleMgr::MChannelRuleMgr()
{

}

MChannelRuleMgr::~MChannelRuleMgr()
{
	Clear();
}


void MChannelRuleMgr::Clear()
{
	m_RuleTypeMap.clear();

	while(!empty())
	{
		MChannelRule* pRule = (*begin()).second;
		delete pRule; pRule = NULL;
		erase(begin());
	}
}

MChannelRule* MChannelRuleMgr::GetRule(const string& strName)
{
	iterator itor = find(strName);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

MChannelRule* MChannelRuleMgr::GetRule(MCHANNEL_RULE nChannelRule)
{
	map<MCHANNEL_RULE, MChannelRule*>::iterator itor = m_RuleTypeMap.find(nChannelRule);
	if (itor != m_RuleTypeMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

void MChannelRuleMgr::ParseRoot(const char* szTagName, MXmlElement* pElement)
{
	if (!stricmp(szTagName, MTOK_CHANNELRULE)) 
	{
		ParseRule(pElement);
	}
}


void MChannelRuleMgr::ParseRule(MXmlElement* pElement)
{
	// Get Rule Node
	int nID = 0;
	pElement->GetAttribute(&nID, MTOK_CHANNELRULE_ATTR_ID);
	char szName[128]="";
	pElement->GetAttribute(szName, MTOK_CHANNELRULE_ATTR_NAME);	

	MChannelRule* pRule = new MChannelRule;
	pRule->Init(nID, szName);

	// Get Map Nodes
	MXmlElement childElement;
	char szTagName[256]=""; char szAttr[256]="";

	int nCount = pElement->GetChildNodeCount();
	for (int i=0; i<nCount; i++) {
		childElement = pElement->GetChildNode(i);

		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_CHANNELMAP))
		{
			if (childElement.GetAttribute(szAttr, MTOK_CHANNELRULE_ATTR_NAME))
			{
				pRule->AddMap(szAttr);
			}
		}
		else if (!stricmp(szTagName, MTOK_CHANNELRULE_GAMETYPE))
		{
			int nAttr = -1;
			if (childElement.GetAttribute(&nAttr, MTOK_CHANNELRULE_ATTR_ID))
			{
				pRule->AddGameType(nAttr);
			}
		}
	}

	AddRule(pRule);
}


void MChannelRuleMgr::AddRule(MChannelRule* pRule)
{
	insert(value_type(pRule->GetName(), pRule));

	if (!stricmp(MCHANNEL_RULE_NOVICE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_NOVICE, pRule));
	}
	else if (!stricmp(MCHANNEL_RULE_NEWBIE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_NEWBIE, pRule));
	}
	else if (!stricmp(MCHANNEL_RULE_ROOKIE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_ROOKIE, pRule));
	}
	else if (!stricmp(MCHANNEL_RULE_MASTERY_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_MASTERY, pRule));
	}
	else if (!stricmp(MCHANNEL_RULE_ELITE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_ELITE, pRule));
	}
	else
	{
		_ASSERT(0);		// 그런 룰은 존재하지 않음.
	}

}