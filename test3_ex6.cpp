//
// Created by fried on 6/10/22.
//

#include "HashMap.hpp"
#include <string>
#include <map>
#include <iostream>


#define test(condition) if (!(condition)) throw std::runtime_error("assert(" #condition ")");

bool passed = true;

void run_test (void (* const test_ptr)(), const std::string &test_name)
{
  std::cout << "running test \"" << test_name << "\": ";
  try
    {
      test_ptr();
      std::cout << "PASSED" << std::endl;
    }
  catch (std::exception &err)
    {
      std::cout << "FAILED - \"" << err.what() << '"' << std::endl;
      passed = false;
    }
}

void test_success ()
{
}

void test_failure ()
{
  throw std::runtime_error("test failed successfully");
}

void test_hashmap_constructors_and_getters()
{
  HashMap<int, int> a;
  test (a.size () == 0);
  test (a.capacity () == 16);
  test (a.empty ());

  const HashMap<std::string, float> b ({"a", "b", "c"}, {1.0f, -0.34f, 11.f});
  test (b.size () == 3);
  test (b.capacity () == 16);
  test (!b.empty ());
  test (std::abs (b.get_load_factor () - 3.f / 16.f) < .1f)

  auto c = b;
  test (c.size () == 3);
  test (c.capacity () == 16);
  test (!c.empty ());

  bool errored = false;
  try
    {
      a.bucket_size (10);
    }
  catch (const std::out_of_range&)
    {
      errored = true;
    }
  catch (const std::invalid_argument&)
    {
      errored = true;
    }
  catch (const std::range_error&)
    {
      errored = true;
    }
  catch (const std::runtime_error&)
    {
      errored = true;
    }
  test (errored);

  errored = false;
  try
    {
      a.bucket_index (10);
    }
  catch (const std::out_of_range&)
    {
      errored = true;
    }
  catch (const std::invalid_argument&)
    {
      errored = true;
    }
  catch (const std::range_error&)
    {
      errored = true;
    }
  catch (const std::runtime_error&)
    {
      errored = true;
    }
  test (errored);

  errored = false;
  try
    {
      HashMap<int, int> m ({1, 2, 3}, {1});
    }
  catch (const std::length_error&)
    {
      errored = true;
    }
  test(errored);

  const HashMap<std::string, float> e ({"a", "b", "c",
                                        "b", "d", "e",
                                        "f", "g"},
                                       {1.0f, -0.34f, 11.f,
                                        2.f, 3.f, 4.f,
                                        5.f, 6.f});
  test (e.bucket_index ("d") == 5);
  test (e.bucket_size ("c") == 2);
  test (e.bucket_size ("b") == 2);
  test (e.bucket_size ("a") == 1);
}

void test_hashmap_insert_get()
{
  const HashMap<std::string, float> a({"a", "b", "c",
                                       "b", "d", "e",
                                       "f", "g"},
                                      {1.0f, -0.34f, 11.f,
                                       2.f, 3.f, 4.f,
                                       5.f, 6.f});
  test (a.size() == 7);
  test (a.contains_key ("a"));
  test (a.contains_key ("g"));
  test (a.at("c") == 11.f);
  test (a.at("b") == 2.f);
  //a["l"];
  bool errored = false;
  try
    {
      a.at("l");
    }
  catch (const std::out_of_range&)
    {
      errored = true;
    }
  catch (const std::invalid_argument&)
    {
      errored = true;
    }
  catch (const std::range_error&)
    {
      errored = true;
    }
  catch (const std::runtime_error&)
    {
      errored = true;
    }
  test (errored);

  HashMap<std::string, float> b = a;
  test(b["e"] == 4.f);
  b["l"];
  test(b.contains_key ("l"));
  test(b.size() == 8);
  b["a"] = -1.f;
  test(a.at("a") != -1.f);
  test(a.at("a") == 1.f);

  std::string test_key = "k";
  for (int i = 0; i < 9; ++i)
    {
      b.insert (test_key, (float)i);
      test_key += "k";
    }
  test(b.size() == 17);
  test(b.capacity() == 32);

  b.erase ("b");
  test (!b.contains_key ("b"));
  test(b.size() == 16);
  test(b.get_load_factor() == .5f);

  b.erase ("k");
  b.erase ("kk");
  b.erase ("kkk");
  b.erase ("kkkk");
  b.erase ("kkkkk");
  b.erase ("kkkkkk");
  b.erase ("kkkkkkk");
  b.erase ("kkkkkkkk");

  test(b.size() == 8);
  test(b.capacity() == 32);
  test(b.get_load_factor() == .25f);

  b.erase ("d");
  test(b.size() == 7);
  test(b.capacity() == 16);
  test(std::abs(b.get_load_factor() - 7.f/16.f) < .1);

  HashMap<float, double> d;
  d[2.f];
  test(d.size() == 1);

  test (d.insert (1.f, 0.));
  test (!d.insert (2.f, -3.));
  test (d.size() == 2);

  HashMap<int, int> f;
  test (f.insert (1, 1));
  test (f.erase (1));
  test (f.capacity() == 1);
}

void test_hashmap_iterator()
{
  const HashMap<std::string, float> a({"a", "b", "c",
                                       "b", "d", "e",
                                       "f", "g"},
                                      {1.0f, -0.34f, 11.f,
                                       2.f, 3.f, 4.f,
                                       5.f, 6.f});
  const std::map<std::string, float> res ({{"a", 1.f},
                                           {"b", 2.f},
                                           {"c", 11.f},
                                           {"d", 3.f},
                                           {"e", 4.f},
                                           {"f", 5.f},
                                           {"g", 6.f}});
  for (const auto &i : a)
    {
      auto found = res.find(i.first);
      test (found != res.end());
      test (found->second == i.second);
    }

  HashMap<int, int> b;
  test (b.empty());
  test (b.begin() == b.end());
  b[2];
  auto it = b.begin();
  test (it != b.end());
  test (it->first == 2);
  test (++it == b.end());
  b[2] = 7;
  test (b.begin()->first == 2);
  test (b.begin()->second == 7);
}

void test_hashmap_comparison()
{
  HashMap<double, char> a;
  a[1.15f] = 0;
  a[2.f] = -15;
  a[2.3f] = 23;

  HashMap<double, char> b = a;
  test (a == b);

  HashMap<double, char> c;
  test (a != c);

  HashMap<double, char> d({1.15f, 2.3f, 2.f}, {0, 23, -15});
  test (a == d);
  test (d == a);

  HashMap<double, char> e({1.15f, 2.3f, 2.f}, {0, -15, 23});
  test (e != a);
  test (e != d);

  HashMap<int, int> s1, s2;
  s1.insert (15, 1);
  s2.insert (15, 1);
  s1.erase (15);
  s1.insert (15, 1);
  test (s1 == s2);
}

int main ()
{
  //run_test (test_success, "test_success");
  //run_test (test_failure, "test_failure");
  run_test (test_hashmap_constructors_and_getters, "test_hashmap_constructors_and_getters");
  run_test (test_hashmap_insert_get, "test_hashmap_insert_get");
  run_test (test_hashmap_iterator, "test_hashmap_iterator");
  run_test (test_hashmap_comparison, "test_hashmap_comparison");
  return (passed) ? EXIT_SUCCESS : EXIT_FAILURE;
}
