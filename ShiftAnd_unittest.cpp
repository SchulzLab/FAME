
#include "gtest/gtest.h"


#include "ShiftAnd.h"


// test fixture
class ShiftAnd_test : public testing::Test {

    protected:


        virtual void SetUp()
        {

            lmap['A'] = 0;
            lmap['C'] = 1;
            lmap['G'] = 2;
            lmap['T'] = 3;
        }



        std::array<uint8_t, 256> lmap;

};


// tests the reset function for resetting the set of active states
// which are represented by bitvectors internally
TEST_F(ShiftAnd_test, reset)
{

    std::string seq = "ACCATGTGACTGCATG";

    ShiftAnd<0> sa0(seq, lmap);

    sa0.active[0].B_0 = 15;
    sa0.active[0].B_1 = 1;

    sa0.reset();

    ASSERT_EQ(1, sa0.active[0].B_0);
    ASSERT_EQ(0, sa0.active[0].B_1);

    ShiftAnd<2> sa2(seq, lmap);

    sa2.active[0].B_0 = 15;
    sa2.active[0].B_1 = 1;
    sa2.active[1].B_0 = 1;
    sa2.active[1].B_1 = 1;
    sa2.active[2].B_0 = 0;
    sa2.active[2].B_1 = 1;

    sa2.reset();

    ASSERT_EQ(1, sa2.active[0].B_0);
    ASSERT_EQ(0, sa2.active[0].B_1);
    ASSERT_EQ(3, sa2.active[1].B_0);
    ASSERT_EQ(0, sa2.active[1].B_1);
    ASSERT_EQ(7, sa2.active[2].B_0);
    ASSERT_EQ(0, sa2.active[2].B_1);


}


// tests if the bitmasks are set correctly for the simple sequence
// ACACACCCC
TEST_F(ShiftAnd_test, simpleBitmasks)
{
    std::string seq = "ACACACCCC";

    ShiftAnd<1> sa1(seq, lmap);

    const uint64_t maskA_0 = sa1.masks[lmap['A']].B_0;
    const uint64_t maskA_1 = sa1.masks[lmap['A']].B_1;
    const uint64_t maskC_0 = sa1.masks[lmap['C']].B_0;
    const uint64_t maskC_1 = sa1.masks[lmap['C']].B_1;
    const uint64_t maskG_0 = sa1.masks[lmap['G']].B_0;
    const uint64_t maskG_1 = sa1.masks[lmap['G']].B_1;
    const uint64_t maskT_0 = sa1.masks[lmap['T']].B_0;
    const uint64_t maskT_1 = sa1.masks[lmap['T']].B_1;

    const uint64_t full = 0xffffffffffffffffULL;
    ASSERT_EQ(0xfffffffffffffc2bULL, maskA_0);
    ASSERT_EQ(0xffffffffffffffd5ULL, maskC_0);
    ASSERT_EQ(0xfffffffffffffc01ULL, maskG_0);
    ASSERT_EQ(0xfffffffffffffc01ULL, maskT_0);
    ASSERT_EQ(full, maskA_1);
    ASSERT_EQ(full, maskC_1);
    ASSERT_EQ(full, maskG_1);
    ASSERT_EQ(full, maskT_1);

    ASSERT_EQ(0x0000000000000200ULL, sa1.accepted.B_0);
    ASSERT_EQ(0, sa1.accepted.B_1);
}

// tests if the bitmasks are set correctly for the simple sequence
// AAAAA.......ACG
// where the last 3 letters are at position 63 to 65 in the sequence
// to test for correct splitting between the two uint64_t
TEST_F(ShiftAnd_test, simpleBitmasksOverflow)
{

    std::string seq = "AAAAAAAAAA";
    seq = seq + seq + seq + seq + seq + seq + "AAAACG";

    ShiftAnd<1> sa1(seq, lmap);

    const uint64_t maskA_0 = sa1.masks[lmap['A']].B_0;
    const uint64_t maskA_1 = sa1.masks[lmap['A']].B_1;
    const uint64_t maskC_0 = sa1.masks[lmap['C']].B_0;
    const uint64_t maskC_1 = sa1.masks[lmap['C']].B_1;
    const uint64_t maskG_0 = sa1.masks[lmap['G']].B_0;
    const uint64_t maskG_1 = sa1.masks[lmap['G']].B_1;
    const uint64_t maskT_0 = sa1.masks[lmap['T']].B_0;
    const uint64_t maskT_1 = sa1.masks[lmap['T']].B_1;

    const uint64_t full = 0xffffffffffffffffULL;
    ASSERT_EQ(full, maskA_0);
    ASSERT_EQ(1, maskC_0);
    ASSERT_EQ(1, maskG_0);
    ASSERT_EQ(1, maskT_0);
    ASSERT_EQ(0xfffffffffffffff9ULL, maskA_1);
    ASSERT_EQ(0xfffffffffffffffaULL, maskC_1);
    ASSERT_EQ(0xfffffffffffffffcULL, maskG_1);
    ASSERT_EQ(0xfffffffffffffff8ULL, maskT_1);

    ASSERT_EQ(0, sa1.accepted.B_0);
    ASSERT_EQ(0x0000000000000004ULL, sa1.accepted.B_1);
}

// tests if too long patterns are still correctly processed
TEST_F(ShiftAnd_test, bitmaskLongPattern)
{
    std::string seq = "GGGGGGGGGGGGGGGG";
    seq += seq;
    seq += seq;
    std::string seq2 = "CCCCCCCCCCCCCCCC";
    seq2 += seq2;
    seq2 += seq2;

    // after the following line, we reached 128 letters
    seq += seq2;

    seq += "AAA";

    ShiftAnd<2> sa2(seq, lmap);

    const uint64_t maskA_0 = sa2.masks[lmap['A']].B_0;
    const uint64_t maskA_1 = sa2.masks[lmap['A']].B_1;
    const uint64_t maskC_0 = sa2.masks[lmap['C']].B_0;
    const uint64_t maskC_1 = sa2.masks[lmap['C']].B_1;
    const uint64_t maskG_0 = sa2.masks[lmap['G']].B_0;
    const uint64_t maskG_1 = sa2.masks[lmap['G']].B_1;
    const uint64_t maskT_0 = sa2.masks[lmap['T']].B_0;
    const uint64_t maskT_1 = sa2.masks[lmap['T']].B_1;

    const uint64_t full = 0xffffffffffffffffULL;
    ASSERT_EQ(1, maskA_0);
    ASSERT_EQ(1, maskC_0);
    ASSERT_EQ(full, maskG_0);
    ASSERT_EQ(1, maskT_0);
    ASSERT_EQ(0, maskA_1);
    ASSERT_EQ(0xfffffffffffffffeULL, maskC_1);
    ASSERT_EQ(1, maskG_1);
    ASSERT_EQ(0, maskT_1);

    ASSERT_EQ(0, sa2.accepted.B_0);
    ASSERT_EQ(0x8000000000000000ULL, sa2.accepted.B_1);
}

// tests if the bitmasks are set correctly for the sequence
// ATTATTTCCC
// to look if WGBS specific behaviour is achieved
TEST_F(ShiftAnd_test, bitmaskBisulfite)
{

    std::string seq = "ATTATTTCCC";

    ShiftAnd<1> sa1(seq, lmap);

    const uint64_t maskA_0 = sa1.masks[lmap['A']].B_0;
    const uint64_t maskC_0 = sa1.masks[lmap['C']].B_0;
    const uint64_t maskG_0 = sa1.masks[lmap['G']].B_0;
    const uint64_t maskT_0 = sa1.masks[lmap['T']].B_0;

    ASSERT_EQ(0xfffffffffffff813ULL, maskA_0);
    ASSERT_EQ(0xffffffffffffffedULL, maskC_0);
    ASSERT_EQ(0xfffffffffffff801ULL, maskG_0);
    ASSERT_EQ(0xfffffffffffff8edULL, maskT_0);
}


// simple matching test with
// same sequence for text as for pattern
TEST_F(ShiftAnd_test, matching_same)
{

    // string of size 20
    std::string seq = "AAAAAAAAAAAAAAAAAAAA";
    seq = seq + seq + seq;
    seq += seq;
    seq += "TTTTTTT";
    // don't allow errors
    ShiftAnd<0> sa0(seq, lmap);
    ShiftAnd<1> sa1(seq, lmap);

    std::vector<unsigned int> matchings0 = sa0.querySeq(seq.begin(), seq.end());
    std::vector<unsigned int> matchings1 = sa1.querySeq(seq.begin(), seq.end());

    ASSERT_EQ(1, matchings0.size());
    ASSERT_EQ(1, matchings1.size());
    ASSERT_EQ(127, matchings0[0]);
    ASSERT_EQ(127, matchings1[0]);

    seq[5] = 'C';

    matchings0 = sa0.querySeq(seq.begin(), seq.end());
    matchings1 = sa1.querySeq(seq.begin(), seq.end());

    ASSERT_EQ(0, matchings0.size());
    ASSERT_EQ(1, matchings1.size());
    ASSERT_EQ(127, matchings1[0]);

}
