#ifndef __MK_ADO_HPP__
#define __MK_ADO_HPP__

#define WIN32_LEAN_AND_MEAN
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF","EndOfFile")
#include <Windows.h>
#include <wx/wx.h>
#include <wx/tokenzr.h>
#include <wx/regex.h>
#include <boost/unordered_map.hpp>
#include <boost/container/vector.hpp>
using namespace std;


struct MKADO
{
	void*           parent;
	_bstr_t         strconnect;
	_bstr_t         strexe;
	_ConnectionPtr  connection  = NULL;
	_RecordsetPtr   recordset   = NULL;
	wxString        time_access = wxEmptyString;
};

MKADO* mkado_create(void* parent)
{
	MKADO* c = NULL;
	try
	{
		CoInitialize(NULL);
		c = new MKADO;
		c->parent      = parent;
		c->connection.CreateInstance(__uuidof(Connection));
		c->recordset   = NULL;
		c->time_access = now_string();
	}	
	catch (...)
	{
		cerr<<"MKADO Create"<<endl<<flush;
	}
	return c;
}

void  mkado_delete(MKADO* c)
{
	try
	{
		c->connection->Close();
		CoUninitialize();
	}
	catch (...)
	{
		cerr<<"DB delete error"<<endl<<flush;
	}
}

bool mkado_connect_sql(MKADO* c,wxString provider,wxString server,wxString db,wxString uid,wxString pwd)
{
	bool ret = false;
	try
	{
		HRESULT  hr;
		wxString strcon = wxString::Format("Provider=%s;Server=%s;Database=%s;UID=%s;PWD=%s;DataTypeCompatibility=80;",provider,server,db,uid,pwd);
		c->strconnect = strcon.wc_str();
		hr = c->connection->Open(c->strconnect,"","",adConnectUnspecified);
		wprintf(L"%s  :  DB Connect Success\n",now_string().wc_str());
		ret = true;
	}
	catch(_com_error& e)
	{
		_bstr_t src (e.Source());
		_bstr_t desc(e.Description());
		wprintf(L"%s  :  DB Connect error :[%08lx] %s : %s : %s\n",now_string().wc_str(),e.Error(),e.ErrorMessage(),(LPCTSTR)src,(LPCTSTR)desc); 
		ret = false;
	}
	return ret;
}

bool mkado_connect_db2(MKADO* c,wxString server,int32_t port,wxString db,wxString uid,wxString pwd)
{
	bool ret = false;
	try
	{
		HRESULT  hr;
		wxString strcon = wxString::Format("Provider=IBMDADB2;Protocol=TCPIP;Hostname=%s;Port=%d;Database=%s;UID=%s;PWD=%s;",server,port,db,uid,pwd);
		c->strconnect = strcon.wc_str();
		hr = c->connection->Open(c->strconnect,"","",adConnectUnspecified);
		wprintf(L"%s  :  DB Connect Success\n",now_string().wc_str());
		ret = true;
	}
	catch(_com_error& e)
	{
		_bstr_t src (e.Source());
		_bstr_t desc(e.Description());
		wprintf(L"%s  :  DB Connect error :[%08lx] %s : %s : %s",now_string().wc_str(),e.Error(),e.ErrorMessage(),(LPCTSTR)src,(LPCTSTR)desc); 
		ret = false;
	}
	return ret;
}



// Read from current record at index
wxString mkado_read(_RecordsetPtr record,wxString index)
{
	wxString ret = wxEmptyString;
	try
	{
		_bstr_t idx = index.wc_str();
		_bstr_t str = record->Fields->Item[idx]->Value;
		ret = wxString((char*)str);
	}
	catch(...)
	{
		ret = wxEmptyString;
	}
	return ret;
}

wxString mkado_read_n(_RecordsetPtr record,long index)
{
	wxString ret = wxEmptyString;
	try
	{		
		_bstr_t str = record->Fields->Item[long(index)]->Value;
		ret = wxString((char*)str);
	}
	catch(...)
	{
		ret = wxEmptyString;
	}
	return ret;
}

// SQL Run return result
wxString mkado_sql(MKADO* c,wxString strsql)
{
	wxString ret = wxEmptyString;
	try
	{
		c->time_access = now_string();
		c->strexe = strsql.wc_str();
		c->recordset = c->connection->Execute(c->strexe, NULL, adCmdText);
		ret = ret + "(";				
		if (c->recordset->State != adStateClosed)
		{
			if (!(c->recordset->EndOfFile) && !(c->recordset->BOF))
			{
				c->recordset->MoveFirst();
				while (!(c->recordset->EndOfFile))
				{
					ret = ret + "(";
					for (long i = 0; i < c->recordset->Fields->GetCount(); i++)
					{
						ret = ret + "\"" + mkado_read_n(c->recordset, i) + "\"" + " ";
					}
					c->recordset->MoveNext();
					ret = ret + ")";
				}
			}
			c->recordset->Close();
		}
		ret = ret + ")";	
		//wprintf(L"%s  :  DB SQL Success\n",now_string().wc_str());
	}
	catch (_com_error &e)
	{
		_bstr_t src (e.Source());
		_bstr_t desc(e.Description());
		wprintf(L"%s  :  DB SQL error :[%08lx] %s : %s : %s",now_string().wc_str(),e.Error(),e.ErrorMessage(),(LPCTSTR)src,(LPCTSTR)desc); 
		ret = wxEmptyString;
	}
	return ret;
}




void mkado_parse (boost::container::vector<boost::container::vector<wxString>>& vv, wxString& s)
{
	// empty
	for(uint32_t i=0;i<vv.size();i++) vv[i].clear();
	vv.clear();

	// fill
	wxRegEx strspace ="[^ ]+";
	wxStringTokenizer tokenizer(s,"()");
	while(tokenizer.HasMoreTokens())
	{
		wxString record = tokenizer.GetNextToken();
		if (record.length() > 0)
		{
			wxStringTokenizer tokenizer2(record, "\"");
			boost::container::vector<wxString>  fields;
			if    (tokenizer2.HasMoreTokens()) tokenizer2.GetNextToken(); // skip first token
			while (tokenizer2.HasMoreTokens())
			{
				wxString field = tokenizer2.GetNextToken();
				bool spacep = !(strspace.Matches(field));
				if (field.length()>0 && !spacep)
				{
					fields.push_back(field);
				}
				else if (field.length()==0 && spacep)
				{
					fields.push_back(wxT("NIL"));
				}
			}
			if (fields.size()>0) vv.push_back(fields);
		}
	}
}

void mkado_print (boost::container::vector<boost::container::vector<wxString>>& vv)
{
	for(uint32_t i=0;i<vv.size();i++)
	{
		for(uint32_t j=0;j<vv[i].size();j++)
		{
			cout<<vv[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<flush;
}



#endif