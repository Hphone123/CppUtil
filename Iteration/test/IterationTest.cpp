#include "../src/Iteration.hpp"

#include "CatchVer.hpp"

using namespace CppUtil;

class IterationTest : public Iterable<int>
{
private:
  int start;
  int endi;

public:
  IterationTest(int start, int end) : start(start), endi(end) {}
  Iterator<int> begin() const override
  {
    return Iterator<int>(this->start);
  }

  Iterator<int> end() const override
  {
    return Iterator<int>(this->endi);
  }
};

class IterationListTest : public Iterable<IterationListTest *>, public IterationElement<IterationListTest *>
{
public:
  int                 val;
  IterationListTest * next = nullptr;

public:
  IterationListTest(int val) : val(val), next(nullptr) {}
  void setNext(IterationListTest * next)
  {
    this->next = next;
  }

  Iterator<IterationListTest *> begin() const override
  {
    return Iterator<IterationListTest *>((IterationListTest *)this);
  }

  Iterator<IterationListTest *> end() const override
  {
    return Iterator<IterationListTest *>((IterationListTest *)nullptr);
  }

  IterationListTest * operator++() override
  {
    return this->next;
  }
};

TEST_CASE("Iteration works", "[iteration]")
{
  IterationTest it(0, 10);

  int sum = 0;
  for (auto v : it)
  {
    sum += v;
  }

  REQUIRE(sum == 45);
}

TEST_CASE("Iteration with pointers works", "[iteration][pointer]")
{
  IterationListTest n1(1);
  IterationListTest n2(2);
  IterationListTest n3(3);
  IterationListTest n4(4);
  IterationListTest n5(5);

  n1.setNext(&n2);
  n2.setNext(&n3);
  n3.setNext(&n4);
  n4.setNext(&n5);

  int sum = 0;
  for (auto v : n1)
  {
    sum += v->val;
  }

  REQUIRE(sum == 15);
}