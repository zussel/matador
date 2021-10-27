#include "SequencerTestUnit.hpp"

#include "matador/utils/sequencer.hpp"
#include "matador/utils/sequence_synchronizer.hpp"
#include "matador/utils/identifier.hpp"

using namespace matador;

SequencerTestUnit::SequencerTestUnit()
  : unit_test("sequencer", "sequencer test unit")
{
  add_test("init", [this] { test_init(); }, "init sequencer");
  add_test("inc", [this] { test_inc(); }, "increment sequencer");
  add_test("reset", [this] { test_reset(); }, "reset sequencer");
  add_test("exchange", [this] { test_exchange(); }, "exchange sequencer");
  add_test("sync", [this] { test_reset(); }, "sync sequencer");
}

void SequencerTestUnit::test_init()
{
  matador::sequencer seq;

  UNIT_ASSERT_EQUAL(seq.init(), 0UL);
}

void SequencerTestUnit::test_inc()
{
  matador::sequencer seq;

  seq.init();

  UNIT_ASSERT_EQUAL(seq.current(), 0UL);
  UNIT_ASSERT_EQUAL(seq.next(), 1UL);
  UNIT_ASSERT_EQUAL(seq.update(5), 5UL);
}

void SequencerTestUnit::test_reset()
{
  matador::sequencer seq;

  seq.init();

  UNIT_ASSERT_EQUAL(seq.current(), 0UL);
  UNIT_ASSERT_EQUAL(seq.next(), 1UL);
  UNIT_ASSERT_EQUAL(seq.reset(3), 3UL);
}

void SequencerTestUnit::test_exchange()
{
  matador::sequencer seq;

  seq.init();

  UNIT_ASSERT_EQUAL(seq.current(), 0UL);
  UNIT_ASSERT_EQUAL(seq.next(), 1UL);

  matador::sequencer_impl_ptr another_seq_ptr(new matador::default_sequencer);

  auto old_seq_ptr = seq.exchange_sequencer(another_seq_ptr);

  UNIT_ASSERT_EQUAL(seq.current(), 0UL);
  UNIT_ASSERT_EQUAL(seq.next(), 1UL);
}

void SequencerTestUnit::test_sync()
{
  sequencer seq;

  seq.init();

  UNIT_ASSERT_EQUAL(seq.current(), 0UL);
  UNIT_ASSERT_EQUAL(seq.next(), 1UL);

  auto pk = make_identifier(7);

  sequence_synchronizer synchronizer(seq);

  synchronizer.sync(pk);

  UNIT_ASSERT_EQUAL(seq.current(), 7UL);
}
