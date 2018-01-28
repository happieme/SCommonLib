#include <string>
#include <iostream>
#include <vector>
#include "../../CommonLib/CommonLibSrc/SString/String.h"
#include "../../CommonLib/CommonLibSrc/SFile/SFile.h"
#include "../../CommonLib/CommonLibSrc/STime/STime.h"

namespace smm
{
	using std::vector;
	using std::cout;
	using std::endl;

	class SExtract
	{
	private:
		String cmd_string_;
		String cmd_param_;
		String cmd_help_string_;
		String prompt_;
		String file_now_;
		void init()
		{
			cmd_string_ = "open, get, grep, history, clear, help";
			prompt_ = "This tool will extract content from a log file\nread row by row";
			cmd_help_string_ = "all command:\n"
				"	open <file path> // If there's space, please use the \" to quote them.\n"
				"	get -with <with string> -without <without string> // include or exclude sth.\n"
				"	grep <...> // You can use grep grammar, but only work on each row.\n"
				"	exit // exit\n"
				"	history // All files that are visited will be listed.\n"
				"	clear -all|<file> // clear all files or one file including the related files.\n"
				"	help // show these help content above.";
		}
	public:
		SExtract()
		{
			init();
		}
		void cmd_help()
		{
			cout << cmd_help_string_.ToCStr() << endl;
		}
		void cmd_open(const StrImg& path)
		{
			SFile sf(path);
			if (sf.Open())
			{
				cout << "File open successful!" << endl;
				sf.Close();
				file_now_ = path;
			}
			else
			{
				cout << "File open failed!" << endl;
			}
		}
		bool cmd_grep(const StrImg& param)
		{
			return true;
		}
		bool cmd_get(const StrImg& param)
		{
			cout << "File: " << file_now_.ToCStr() << ", search string= "<< param.ToCStr() << endl;
			if (file_now_.IsEmpty())
			{
				cout << "No file open! Open a file first." << endl;
				return false;
			}
			SFile sf(file_now_);
			cout << param.ToCStr() << endl;
			if (!sf.Open("r"))
			{
				cout << "File: " << file_now_.ToCStr() << " open failed!" << endl;
				return false;
			}
			vector<String> res;
			String strRow;
			Uint total_size = 0;
			while(!sf.IsEnd())
			{
				sf.ReadLine(strRow);
				if (TheEndPos != strRow.Find(param))
				{
					res.push_back(strRow);
					total_size += strRow.Size();
				}
			}
			if (!res.empty())
			{
				SFilePath sph(file_now_);
				SARRAY(res_file_name, 128);
				res_file_name.Append(sph.GetFileName());
				STime st;
				SARRAY(tStr, 64);
				tStr.FormatStr("_%lld", st.TimeStamp());
				res_file_name.Append(tStr);
				cout << "The result is stored in file: " << res_file_name.ToCStr() << endl;
				bool cmd_show = false;
				if (total_size < 1024 * 2)
				{
					cmd_show = true;
					cout << "The result size is smaller than 2 k, will show here:\n" << endl;
				}
				SFile res_file(res_file_name);
				if (res_file.Open("ab+"))
				{
					for (Uint n = 0; n < res.size(); n++)
					{
						if (cmd_show)
						{
							cout << res.at(n).ToCStr() << endl;
						}
						res_file.Write(res.at(n));
					}
				}
				return true;
			}
			else
			{
				cout << "Failed find the search string:" << param.ToCStr() << endl;
				return false;
			}
		}
		bool cmd_resolve(const StrImg& tmp)
		{
			cmd_param_ = tmp;
			StrImg str_cmd; // 命令本身，后面是参数
			StrImg str_param; // 命令的参数
			Uint space_pos = cmd_param_.Find(" ");
			if (space_pos != TheEndPos)
			{
				str_cmd = cmd_param_.Sub(0, space_pos);
				str_param = cmd_param_.Sub(space_pos + 1);
			}
			if (str_cmd.IsEmpty() || TheEndPos == cmd_string_.Find(str_cmd))
			{
				throw "Non-exist command!";
			}
			if (str_cmd.IsEqual("open"))
			{
				StrImg cmd_1 = str_param;
				if (!cmd_1.IsEmpty())
				{
					cmd_open(cmd_1);
				}
			}
			else if (str_cmd.IsEqual("get"))
			{
				Uint pos = cmd_param_.Find(str_cmd);
				if (cmd_param_.Size() > 3 && ' ' == cmd_param_.GetAt(3))
				{
					StrImg str = cmd_param_.Sub(4);
					cmd_get(str);
				}
			}
			else if (str_cmd.IsEqual("help"))
			{
				cmd_help();
			}
			else if (str_cmd.IsEqual("exit"))
			{
				return false;
			}
			else
			{
				cmd_help();
				throw "Non-exist command! See help.";
			}
			return true;
		}
	};
	void extract_main()
	{
		SExtract sext;
		std::string input;
		try
		{
			sext.cmd_resolve("open D:\\MyFile\\mysql-src\\mysql-make\\data\\Data\\HAPPIEME-PC.err");
			sext.cmd_resolve("get Innodb");
			while (1)
			{
				getline(cin, input);
				if (!input.empty())
				{
					String cmd(input.c_str());
					cmd.Trim();
					if (!sext.cmd_resolve(cmd))
					{
						break;
					}
				}
			}
		}
		catch (const char* err)
		{
			cout << "Error throw out: " << err << endl;
		}
	}
};
