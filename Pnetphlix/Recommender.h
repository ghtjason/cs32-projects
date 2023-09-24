#ifndef RECOMMENDER_INCLUDED
#define RECOMMENDER_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>

class UserDatabase;
class MovieDatabase;
class Movie;

struct MovieAndRank
{
    MovieAndRank(const std::string& id, int score)
     : movie_id(id), compatibility_score(score)
    {}

    std::string movie_id;
    int compatibility_score;
};

class Recommender
{
  public:
    Recommender(const UserDatabase& user_database,
                const MovieDatabase& movie_database);
    std::vector<MovieAndRank> recommend_movies(const std::string& user_email,
                                               int movie_count) const;

  private:
    // increments the int each movie* is mapped to by scoreIncrement
    void mapMoviesToScore(const std::vector<Movie*> movies, std::unordered_map<Movie*,int>&, int scoreIncrement) const;
    const UserDatabase* m_udb;
    const MovieDatabase* m_mdb;
};

#endif // RECOMMENDER_INCLUDED
