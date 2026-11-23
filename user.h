#ifndef USER_H
#define USER_H

#include "common.h"

// Account Management
////////////////////////////////////////////////////////////////////////
class AccountManager 
{
    public:
        deque<UserAccount> accounts;

        void load() 
        {
            accounts.clear();
            ifstream f(DB_FILE);
            if (!f.is_open())
            {
                return;
            }
            string line;
            while (getline(f, line)) 
            {
                if (line.empty())
                {
                    continue;
                }
                istringstream ss(line);
                string tok;
                vector<string> parts;
                while (getline(ss, tok, ','))
                {
                    parts.push_back(tok);
                }
                UserAccount a;
                a.username = parts[0];
                a.password = parts[1];
                a.p1ColorIdx = stoi(parts[2]);
                a.p2ColorIdx = stoi(parts[3]);
                if (parts.size() >= 8) 
                {
                    a.stats.wins = stoi(parts[4]);
                    a.stats.losses = stoi(parts[5]);
                    a.stats.draws = stoi(parts[6]);
                    a.stats.total = stoi(parts[7]);
                }
                accounts.push_back(a);
            }
        }

        void save() 
        {
            ofstream f(DB_FILE, ios::trunc);
            for (auto& a : accounts)
            {
                f << toLower(a.username) << "," << a.password << ","
                << a.p1ColorIdx << ","   << a.p2ColorIdx      << ","
                << a.stats.wins << ","   << a.stats.losses    << ","
                << a.stats.draws << ","  << a.stats.total     << "\n";
            }
        }

        UserAccount* login(const string& user, const string& pass) 
        {
            for (auto& a : accounts)
            {
                if (a.username == toLower(user) && a.password == pass)
                {
                    return &a;
                }
            }
            return nullptr;
        }

        bool registerUser(const string& user, const string& pass) 
        {
            for (auto& a : accounts)
            {
                if (a.username == user)
                {
                    return false;
                }
            }
            UserAccount a;
            a.username = toLower(user);
            a.password = pass;
            accounts.push_back(a);
            save();
            return true;
        }

        void saveAccount(const UserAccount& updated) 
        {
            for (auto& a : accounts)
            {
                if (a.username == updated.username) 
                { 
                    a = updated; 
                    break;
                }
            }
            save();
        }
};
////////////////////////////////////////////////////////////////////////

// History Management
////////////////////////////////////////////////////////////////////////
class HistoryManager 
{
public:
    vector<MatchRecord> records;

    void load() 
    {
        records.clear();
        ifstream f(HISTORY_FILE);
        if (!f.is_open())
        {
            return;
        }
        string line;
        while (getline(f, line)) 
        {
            if (line.empty())
            {
                continue;
            }
            istringstream ss(line);
            string tok;
            vector<string> parts;

            while (getline(ss, tok, '|'))
            {
                if (!tok.empty() && tok.back() == '\r')
                {
                    tok.pop_back();
                }
                parts.push_back(tok);
            }
            if (parts.size() < 4)
            {
                continue;
            }
            MatchRecord r;
            r.p1Name = parts[0];
            r.p2Name = parts[1];
            r.result = parts[2];
            r.timeTaken = parts[3];
            records.push_back(r);
        }
    }

    void save() 
    {
        ofstream f(HISTORY_FILE, ios::trunc);
        for (auto& r : records)
        {
            f << r.p1Name << "|" << r.p2Name << "|"
            << r.result << "|" << r.timeTaken << "\n";
        }
    }

    void addRecord(const MatchRecord& r) 
    {
        records.push_back(r);
        save();
    }
};
////////////////////////////////////////////////////////////////////////


#endif
