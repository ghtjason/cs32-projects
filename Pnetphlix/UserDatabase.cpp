#include "UserDatabase.h"
#include "User.h"

#include <string>
#include <vector>
#include <fstream>
using namespace std;

UserDatabase::UserDatabase()
{
    m_fileLoaded = false;
}

UserDatabase::~UserDatabase()
{
    for(int i = 0; i < m_users.size(); i++)
        delete m_users[i];
}

bool UserDatabase::load(const string& filename)
{
    if(m_fileLoaded)
        return false;
    m_fileLoaded = true;
    m_filePath = filename;

    ifstream userFile(m_filePath);
    if(!userFile)
        return false;
    string empty, name, email, movie;
    int movies;
    // parsing users txt file
    while(getline(userFile, name))
    {
        getline(userFile, email);
        if(!(userFile >> movies))       // did not find # of movies at expected line
            return false;
        getline(userFile, empty);
        vector<string> watchHistory;
        for(int i = 0; i < movies; i++)
        {
            getline(userFile, movie);
            watchHistory.push_back(movie);
        }
        getline(userFile, empty);        // ignore empty line to go to next user name
        User* newUser = new User(name, email, watchHistory);
        m_users.push_back(newUser);
        m_tmm.insert(email, newUser);
    }
    return true;
}

User* UserDatabase::get_user_from_email(const string& email) const
{
    TreeMultimap<string,User*>::Iterator it = m_tmm.find(email);
    if(!it.is_valid())
        return nullptr;
    return it.get_value();
}
