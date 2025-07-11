//
// Clark Kromenaker
//
// Tracks game progress/logic variables and the state of the player
// throughout the game. Keeps track of thinks like current/last location/time,
// flag states, game logic variable states, noun/verb counts, etc.
//
#pragma once
#include <functional>
#include <string>

#include "FlagSet.h"
#include "PersistState.h"
#include "StringUtil.h"
#include "Timeblock.h"

class GameProgress
{
public:
    void Init();

    // Score Tracking
    int GetMaxScore() const { return mMaxScore; }
    int GetScore() const { return mScore; }
    void SetScore(int score);
    void IncreaseScore(int points);
    void ChangeScore(const std::string& scoreName);

    // Timeblock
    const Timeblock& GetTimeblock() const { return mTimeblock; }
    void SetTimeblock(const Timeblock& timeblock);

    const Timeblock& GetLastTimeblock() const { return mLastTimeblock; }
    std::string GetTimeblockDisplayName() const;
    std::string GetTimeblockDisplayName(const std::string& timeblockStr) const;

    void EndCurrentTimeblock(const std::function<void()>& callback);
    void StartTimeblock(const Timeblock& timeblock, bool loadingSave, const std::function<void()>& callback);
    bool IsChangingTimeblock() const { return mChangingTimeblock; }

    // Flags
    bool GetFlag(const std::string& flagName) const { return mGameFlags.Get(flagName); }
    void SetFlag(const std::string& flagName) { mGameFlags.Set(flagName); }
    void ClearFlag(const std::string& flagName) { mGameFlags.Clear(flagName); }
    void DumpFlags() const { mGameFlags.Dump("game"); }

    // Game Variables
    int GetGameVariable(const std::string& varName) const;
    void SetGameVariable(const std::string& varName, int value);
    void IncGameVariable(const std::string& varName);

    // Chat Counts
    int GetChatCount(const std::string& noun) const;
    void SetChatCount(const std::string&, int count);
    void IncChatCount(const std::string& noun);

    // Topic Counts
    int GetTopicCount(const std::string& noun, const std::string& topic) const;
    int GetTopicCount(const std::string& actor, const std::string& noun, const std::string& topic) const;
    void SetTopicCount(const std::string& noun, const std::string& topic, int count);
    void SetTopicCount(const std::string& actor, const std::string& noun, const std::string& topic, int count);
    void IncTopicCount(const std::string& noun, const std::string& topic);
    void IncTopicCount(const std::string& actor, const std::string& noun, const std::string& topic);

    // Noun/Verb Counts
    int GetNounVerbCount(const std::string& noun, const std::string& verb) const;
    int GetNounVerbCount(const std::string& actor, const std::string& noun, const std::string& verb) const;
    void SetNounVerbCount(const std::string& noun, const std::string& verb, int count);
    void SetNounVerbCount(const std::string& actor, const std::string& noun, const std::string& verb, int count);
    void IncNounVerbCount(const std::string& noun, const std::string& verb);
    void IncNounVerbCount(const std::string& actor, const std::string& noun, const std::string& verb);

    void OnPersist(PersistState& ps);

private:
    // Score tracking.
    int mMaxScore = 965;
    int mScore = 0;

    // Maps a score change label (e.g. e_110a_r25_tape) to the number of points gained.
    std::string_map_ci<int> mScoreEvents;

    // Tracks which score events the player has already triggered.
    std::string_map_ci<bool> mScoreEventFlags;

    // Current and last time blocks.
    Timeblock mTimeblock;
    Timeblock mLastTimeblock;

    // If true, we are currently changing timeblocks.
    bool mChangingTimeblock = false;

    // General-use true/false flags for game logic.
    FlagSet mGameFlags;

    // Tracks the number of times the player has chatted with a noun.
    std::string_map_ci<int> mChatCounts;

    // Maps noun/topic combos to a count value.
    // Tracks the number of times we've talked to a noun about a topic.
    std::string_map_ci<int> mTopicCounts;

    // Maps noun/verb to a count value.
    // Tracks the number of times we've triggered a verb on a noun.
    std::string_map_ci<int> mNounVerbCounts;

    // Maps a variable name to an integer value.
    // For general game logic variables.
    std::string_map_ci<int> mGameVariables;
};

extern GameProgress gGameProgress;