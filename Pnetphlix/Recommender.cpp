#include "Recommender.h"
#include "UserDatabase.h"
#include "MovieDatabase.h"
#include "User.h"
#include "Movie.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>    // for sort()
#include <utility>      // for pair

using namespace std;

// comparison function to compair two pairs of Movie*/int, based on the criteria in the spec
bool comparePairs(const pair<Movie*, int>& a, const pair<Movie*, int>& b)
{
    if(a.second != b.second)                                // comparing compatability scores
        return a.second > b.second;
    if(a.first->get_rating() != b.first->get_rating())      // comparing star ratings
        return a.first->get_rating() > b.first->get_rating();
    return a.first->get_title() < b.first->get_title();     // comparing titles
}

Recommender::Recommender(const UserDatabase& user_database,
                         const MovieDatabase& movie_database)
{
    m_udb = &user_database;
    m_mdb = &movie_database;
}

vector<MovieAndRank> Recommender::recommend_movies(const string& user_email, int movie_count) const
{
    vector<MovieAndRank> v;
    vector<string> watchHistory = m_udb->get_user_from_email(user_email)->get_watch_history();
    if(watchHistory.empty())
        return v;   // return empty vector is user hasn't watched anything
    unordered_map<Movie*, int> movieToScore;
    // for every movie in watchHistory, get directors/actors/genres
    // then for every movie containing the director/actor/genre, increment their compatability score
    for(int i = 0; i < watchHistory.size(); i++)
    {
        Movie* m = m_mdb->get_movie_from_id(watchHistory[i]);
        vector<string> directors = m->get_directors();
        vector<string> actors = m->get_actors();
        vector<string> genres = m->get_genres();
        for(int j = 0; j < directors.size(); j++)
            mapMoviesToScore(m_mdb->get_movies_with_director(directors[j]), movieToScore, 20);
        for(int j = 0; j < actors.size(); j++)
            mapMoviesToScore(m_mdb->get_movies_with_actor(actors[j]), movieToScore, 30);
        for(int j = 0; j < genres.size(); j++)
            mapMoviesToScore(m_mdb->get_movies_with_genre(genres[j]), movieToScore, 1);
    }
    for(int i = 0; i < watchHistory.size(); i++)    // erase from map if user has already watched
    {
        Movie* m  = m_mdb->get_movie_from_id(watchHistory[i]);
        if(movieToScore.find(m) != movieToScore.end())
            movieToScore.erase(m);
    }
    unordered_map<Movie*, int>::iterator it = movieToScore.begin();
    vector<pair<Movie*, int>> pairVector;
    // process unordered map of movie*/score into a vector of pairs of movie*/score (for sorting)
    while(it != movieToScore.end())
    {
        pairVector.push_back(pair<Movie*, int>(it->first, it->second));
        it++;
    }
    sort(pairVector.begin(), pairVector.end(), comparePairs);   // sort vector based on specified criteria
    int numOfMovies = min(static_cast<int>(pairVector.size()), movie_count);    // decide how many recommendations to return
    for(int i = 0; i < numOfMovies; i++)
        v.push_back(MovieAndRank(pairVector[i].first->get_id(), pairVector[i].second));
    return v;
}

void Recommender::mapMoviesToScore(const vector<Movie*> movies, unordered_map<Movie*,int>& movieToScore, int scoreIncrement) const
{
    for(int i = 0; i < movies.size(); i++)
        movieToScore[movies[i]] += scoreIncrement;
}
