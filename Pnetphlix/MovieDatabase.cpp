#include "MovieDatabase.h"
#include "Movie.h"

#include <string>
#include <vector>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <fstream>
using namespace std;

MovieDatabase::MovieDatabase()
{
    m_fileLoaded = false;
}

MovieDatabase::~MovieDatabase()
{
    for(int i = 0; i < m_movies.size(); i++)
        delete m_movies[i];
}

bool MovieDatabase::load(const string& filename)
{
    if(m_fileLoaded)
        return false;
    m_fileLoaded = true;
    m_filePath = filename;

    ifstream userFile(m_filePath);
    if(!userFile)
        return false;
    string id, title, year, directorList, actorList, genreList, empty;
    float rating;
    // parsing movie txt file
    while(getline(userFile, id))
    {
        getline(userFile, title);
        getline(userFile, year);
        
        getline(userFile, directorList);
        vector<string> directors;
        processCommaSeparatedList(directorList, directors);
        
        getline(userFile, actorList);
        vector<string> actors;
        processCommaSeparatedList(actorList, actors);
        
        getline(userFile, genreList);
        vector<string> genres;
        processCommaSeparatedList(genreList, genres);
        
        if(!(userFile >> rating))       // did not find # of movies at expected line
            return false;
        getline(userFile, empty);
        getline(userFile, empty);        // ignore empty line to go to next user name

        Movie* newMovie = new Movie(id, title, year, directors, actors, genres, rating);
        m_movies.push_back(newMovie);   // for deletion upon destruction
        m_idMap.insert(stringToLower(id), newMovie);
        
        // creating TMMs mapping directors/actors/genres to movies
        processVectorIntoTMM(m_directorMap, newMovie, directors);
        processVectorIntoTMM(m_actorMap, newMovie, actors);
        processVectorIntoTMM(m_genreMap, newMovie, genres);

    }
    return true;
}

Movie* MovieDatabase::get_movie_from_id(const string& id) const
{
    string lowerID = stringToLower(id);
    TreeMultimap<string,Movie*>::Iterator it = m_idMap.find(lowerID);
    if(!it.is_valid())
        return nullptr;
    return it.get_value();
}

vector<Movie*> MovieDatabase::get_movies_with_director(const string& director) const
{
    vector<Movie*> movies;
    processTMMIntoVector(m_directorMap, director, movies);
    return movies;
}

vector<Movie*> MovieDatabase::get_movies_with_actor(const string& actor) const
{
    vector<Movie*> movies;
    processTMMIntoVector(m_actorMap, actor, movies);
    return movies;
}

vector<Movie*> MovieDatabase::get_movies_with_genre(const string& genre) const
{
    vector<Movie*> movies;
    processTMMIntoVector(m_genreMap, genre, movies);
    return movies;
}

string MovieDatabase::stringToLower(string s) const
{
    for(int i = 0; i < s.size(); i++)
    {
        s[i] = tolower(s[i]);
    }
    return s;
}

// insert a list of values separated by commas into v
void MovieDatabase::processCommaSeparatedList(const string &s, vector<string>& v) const
{
    stringstream ss(s);
    string value;
    while(getline(ss, value, ','))
        v.push_back(value);
}

// maps every key inside v to Movie*
void MovieDatabase::processVectorIntoTMM(TreeMultimap<string, Movie*>& tmm, Movie* m, const vector<string>& v)
{
    string key;
    for(int i = 0; i < v.size(); i++)
    {
        key = stringToLower(v[i]);
        tmm.insert(key, m);
    }
}

// inserts all movie pointers mapped to key in the TMM into vector v
void MovieDatabase::processTMMIntoVector(const TreeMultimap<string, Movie*>& tmm, const string& key, std::vector<Movie*>& v) const
{
    string lowerKey = stringToLower(key);
    TreeMultimap<string,Movie*>::Iterator it = tmm.find(lowerKey);
    while(it.is_valid())
    {
        v.push_back(it.get_value());
        it.advance();
    }
}
