#ifndef _HASHMAP_HPP_
#define _HASHMAP_HPP_

#include <memory>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <exception>
#define LOWER_LOAD_FACTOR 1/4
#define UPPER_LOAD_FACTOR 3/4
#define EMPTY_HASH 0
#define STARTING_HASH_CAPACITY 16
#define MINIMUM_VALID_CAPACITY 1
#define CONSTRUCTOR_ERROR "ERROR: can't construct,size of vectors don't match."
#define INVALID_KEY_ERROR "USAGE: given key doesn't exists in the container."
#define INCREASE_HASH 1
#define DECREASE_HASH 0

using std::hash;
using std::vector;
using std::list;
using std::pair;
using std::string;

template<class KeyT, class ValueT>
class HashMap
{
  //Typedefs to simplify the code.
  typedef pair<KeyT,ValueT> item;
  typedef vector<item> bucket;


  //Map fields
 protected:
  bucket *hash_table;
  int map_size;
  double load_factor;
  int map_capacity;

  //Map helper functions
  void update_load_factor ()
  {
    double temp = (double) size() / capacity();
    load_factor = temp;
  }

  int hash_func (const KeyT& key) const
  {
    return hash<KeyT> {} (key) & (map_capacity - 1);
  }

  /**
   * The function rehashes the map by changing its capacity according to
   * direction and then hashes the items back in to the map.
   * @param direction - Orders the function if it needs to be increase the
   * capacity or decrease it.
   */
   void rehash_func(const int direction)
  {
    list<pair<KeyT, ValueT>> temp;
    for (int i = 0;i < map_capacity;i++)
    {
      bucket curr_bucket = *(hash_table + i);
      for (size_t j = 0; j < curr_bucket.size();j++)
      {
        temp.push_back (curr_bucket[j]);
      }
    }
    change_capacity (direction);
    delete[] hash_table;
    map_size = EMPTY_HASH;
    hash_table = new bucket [map_capacity];
    for (const auto& item : temp)
    {
      this->insert (item.first, item.second);
    }
  }

  /**
   * Helper function for the rehash, changes the capacity according to the
   * direction so the hashmap will be at the right size.
   * @param direction - INCREASE_HASH / DECREASE HASH
   */
  void change_capacity(const int direction)
  {
    if (direction == INCREASE_HASH)
    {
      map_capacity *= 2;
    }
    if (direction == DECREASE_HASH)
    {
      if (map_size == EMPTY_HASH)
      {
        map_capacity = MINIMUM_VALID_CAPACITY;
        update_load_factor();
      }
      else
      {
        while (load_factor < (double) LOWER_LOAD_FACTOR)
        {
          map_capacity /= 2;
          update_load_factor();
        }
      }
    }
  }

 public:
  //Default Constructor
  HashMap ():
  hash_table (new bucket[STARTING_HASH_CAPACITY]),
  map_size (EMPTY_HASH),
  load_factor(EMPTY_HASH),
  map_capacity (STARTING_HASH_CAPACITY)
  {};

  /**
   * Constructs a hash-table from a vectors of keys and a vectors of values.
   * @param key_vect
   * @param value_vect
   */
  HashMap (const vector<KeyT>& key_vect, const vector<ValueT> &value_vect):
      hash_table (new bucket[STARTING_HASH_CAPACITY]),
      map_size (EMPTY_HASH),
      load_factor(EMPTY_HASH),
      map_capacity (STARTING_HASH_CAPACITY)
  {
    if (key_vect.size () != value_vect.size ())
    {
      delete[] hash_table;
      throw std::length_error (CONSTRUCTOR_ERROR);
    }
    for (size_t i = 0; i < key_vect.size (); i++)
    {
      operator[] (key_vect[i]) = value_vect[i];
    }
  };


  HashMap (const HashMap &other):
      hash_table(new bucket[other.map_capacity]), map_size(EMPTY_HASH),
      load_factor(other.load_factor), map_capacity(other.map_capacity)
  {
    for (auto item = other.begin(); item != other.end();item++)
    {
      operator[] ((*item).first) = (*item).second;
    }
  }

  virtual ~HashMap ()
  {
    delete[] hash_table;
  };

  int size () const
  { return map_size; }

  int capacity () const
  { return map_capacity; }


  /**
   * @return A boolean value whether the hash-table is empty or not.
   */
  bool empty () const
  { return (map_size == 0); }

  bool insert (const KeyT &key,const ValueT &value)
  {
    if (!contains_key (key))
    {
      //insert a new pair into the hash_table.
      int hash_value = hash_func (key);
      bucket &curr_bucket = *(hash_table + hash_value);
      curr_bucket.emplace_back (key, value);
      curr_bucket.shrink_to_fit();
      map_size++;
      update_load_factor();
      if (load_factor > (double) UPPER_LOAD_FACTOR)
      {
        rehash_func (INCREASE_HASH);
      }
      return true;
    }
    //Key already exists in the hash_table.
    return false;
  }

  /**
  * Erases a value of a given key.
  * @param key: The key of the desired value the user wants to erase.
  * @return A bool value whether the process was successful or not.
  */
  virtual bool erase (const KeyT& key)
  {
    int hash_value = hash_func (key);
    if (!contains_key (key))
    {
      return false;
    }
    bucket &curr_bucket = *(hash_table + hash_value);
    //Iterator on the desired pair<key,value> in the hash map.
    auto it = std::find_if(curr_bucket.begin(), curr_bucket.end(),
                           [&key](const item& element)
                           {return element.first == key;});
    (hash_table + hash_value)->erase (it);
    map_size--;
    update_load_factor();
    if (load_factor < (double)LOWER_LOAD_FACTOR)
    {
      rehash_func(DECREASE_HASH);
    }
    return true;
  }

  /**
   * @param key - The desired key the user is looking for its existence.
   * @return A boolean value whether the key is in the hash table or not.
   */
  bool contains_key (const KeyT& key) const
  {
    int hash_value = hash_func (key);
    for (const auto& item : *(hash_table + hash_value))
    {
      if (item.first == key)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * @param key - The key of the desired value.
   * @return a value from the hash-table by a given key.
   */
  ValueT& at (const KeyT& key)const
  {
    int hash_value = hash_func (key);
    for (auto& item : *(hash_table + hash_value))
    {
      if (item.first == key)
      {
        return item.second;
      }
    }
    throw std::runtime_error (INVALID_KEY_ERROR);
  }

  ValueT& at (const KeyT& key)
  {
    int hash_value = hash_func (key);
    for (auto& item : *(hash_table + hash_value))
    {
      if (item.first == key)
      {
        return item.second;
      }
    }
    throw std::runtime_error (INVALID_KEY_ERROR);
  }

  double get_load_factor () const
  { return load_factor; }


  /**
   * @param key: The key the user wishes to get its bucket size.
   * @return The size of the bucket that contains the key.
   */
  int bucket_size (const KeyT& key) const
  {
    if (!contains_key (key))
    {
      throw std::runtime_error (INVALID_KEY_ERROR);
    }
    int hash_value = hash_func (key);
    return (hash_table + hash_value)->size();
  }

  /**
  * @param key: The key the user wishes to get its bucket index.
  * @return The index of the bucket in the hash-table that contains the key.
  */
  int bucket_index (const KeyT& key) const
  {
    if (!contains_key (key))
    {
      throw std::runtime_error (INVALID_KEY_ERROR);
    }
    return hash_func (key);
  }

  /**
   * Removes all the items in the hash-table but doesn't change its capacity.
   */
  void clear ()
  {
    delete[] hash_table;
    hash_table = new bucket [map_capacity];
    map_size = EMPTY_HASH;
    update_load_factor();
  }

  bool operator==(const HashMap& rhs)const
  {
    //Check the basic parameter before iterating over the map.
    if (map_size != rhs.map_size)
    {
      return false;
    }
    //Iterate to make sure they also contain the same items.
    for (const auto& item : rhs)
    {
      try
      {
        if (at(item.first) != rhs.at (item.first))
        {
          return false;
        }
      }
      catch (std::runtime_error &err)
      {
        return false;
      }
    }
    return true;
  }


  bool operator!=(const HashMap& rhs)const
  {
    return !(operator==(rhs));
  }

  ValueT& operator[](const KeyT& key)
  {
      int hash_value = hash_func (key);
      for (auto& item : *(hash_table + hash_value))
      {
        if (item.first == key)
        {
          return item.second;
        }
      }
      insert (key,ValueT());
      return at (key);
  }

  ValueT operator[](const KeyT& key)const
  {
    int hash_value = hash_func (key);
    for (auto& item : *(hash_table + hash_value))
    {
      if (item.first == key)
      {
        return item.second;
      }
    }
    return ValueT();
  }

  void operator=(const HashMap& rhs)
  {
    if (this == &rhs)
    {
      return;
    }
    this->map_capacity = rhs.map_capacity;
    this->clear();
    update_load_factor();
    for (auto item : rhs)
    {
      operator[] (item.first) = item.second;
      update_load_factor();
      if (load_factor > (double) UPPER_LOAD_FACTOR)
      {
        rehash_func (INCREASE_HASH);
      }
    }
  }



  class ConstIterator
  {
    friend class HashMap;

   public:
    typedef pair<KeyT, ValueT> value_type;
    typedef const value_type &reference;
    typedef const value_type *pointer;
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

   private:
    bucket *hash_map;
    int capacity;
    int inner_index;
    int outer_index;

   public:
    ConstIterator (bucket *_hash_table, int _capacity, int
    _outer_index, int _inner_index):
        hash_map(_hash_table), capacity(_capacity),
        inner_index (_inner_index),outer_index(_outer_index)
    {}

    ConstIterator& operator++ ()
    {
      inner_index++;
      //Need to continue to the next non-empty bucket.
      if ((size_t) inner_index >=  (*(hash_map +outer_index)).size())
      {
        outer_index++;
        while (outer_index != capacity && (*(hash_map +outer_index)).empty())
        {
          outer_index++;
        }
        inner_index = 0;
      }
      return *this;
    }

    ConstIterator operator++ (int)
    {
      ConstIterator it(*this);
      this->operator++();
      return it;
    }

    /**
     * It compares the iterators by checkin that they are pointing to the same
     * pair at the moment and checking the indexes are equal to one another.
     * @param rhs - The other hashmap iterator that the function compares its
     * hashmap
     * to it.
     * @return boolean value whether the iterators are equal or not.
     */
    bool operator== (const ConstIterator &rhs) const
    {
      if (outer_index == rhs.capacity && capacity == rhs.outer_index)
      {
        return true;
      }
      if (outer_index == rhs.outer_index && inner_index == rhs.inner_index)
      {
        //Check the iterators are pointing to the same pair in the hashmap.
        ValueT *this_value_ptr =  &(((*(hash_map + outer_index))
            [inner_index]).second);
        KeyT *this_key_ptr =  &(((*(hash_map + outer_index))
            [inner_index]).first);
        KeyT *rhs_key_ptr =  &(((*(rhs.hash_map + outer_index))
            [inner_index]).first);
        ValueT *rhs_value_ptr =  &(((*(hash_map + outer_index))[inner_index])
            .second);
        return rhs_key_ptr == this_key_ptr && rhs_value_ptr == this_value_ptr;
      }
      return false;
    }

    bool operator!= (const ConstIterator &rhs) const
    {

      return !(operator== (rhs));
    }

    reference operator* () const
    {
      return (*(hash_map + outer_index))[inner_index];
    }

    pointer operator-> () const
    {
      return &(operator*());
    }

  };


  using const_iterator = ConstIterator;

  const_iterator begin() const
  {
    int starting_bucket_idx = 0;

    while (starting_bucket_idx < map_capacity && (*(hash_table +
                                        starting_bucket_idx)).empty())
    {
      starting_bucket_idx++;
    }
    return const_iterator(hash_table,map_capacity,starting_bucket_idx,0);
  }

  const_iterator cbegin() const
  {
    return begin();
  }

  const_iterator end() const
  {
    return const_iterator(hash_table,map_capacity,map_capacity,0);
  }

  const_iterator cend() const
  {
    return end();
  }


};

#endif //_HASHMAP_HPP_


