#ifndef _DICTIONARY_HPP_
#define _DICTIONARY_HPP_
#include "HashMap.hpp"

class InvalidKey: public std::invalid_argument
{
 public:

  InvalidKey(): std::invalid_argument (INVALID_KEY_ERROR)
  {}

  explicit InvalidKey(const std::string &error):
      std::invalid_argument (error){}
};


 class Dictionary : public HashMap<std::string ,std::string>
{

 public:

  Dictionary(){};
  Dictionary(vector<string> key_vect, vector<string> value_vect):
      HashMap(key_vect,value_vect){};

  Dictionary(const Dictionary &other): HashMap(other){};

    bool erase(const std::string &key) override
   {
    if (!contains_key (key))
    {
      throw InvalidKey(INVALID_KEY_ERROR);
    }
    return HashMap<std::string ,std::string>::erase (key);
   }

  template<class DictIterator>
  void update (DictIterator begin, const DictIterator &end)
  {
    while (begin != end)
    {
      operator[]((*begin).first) = (*begin).second;
      if ((double) (size() / capacity()) > (double) UPPER_LOAD_FACTOR)
      {
        rehash_func (INCREASE_HASH);
      }
      begin++;
    }
  }
};




#endif //_DICTIONARY_HPP_
