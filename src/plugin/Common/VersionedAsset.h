#pragma once
#include <string>
#include <iostream>

enum State
{
	kNone = 0,
	kLocal = 1 << 0,
	kSynced = 1 << 1,
	kOutOfSync = 1 << 2,
	kMissing = 1 << 3,
	kCheckedOutLocal = 1 << 4,
	kCheckedOutRemote = 1 << 5,
	kDeletedLocal = 1 << 6,
	kDeletedRemote = 1 << 7,
	kAddedLocal = 1 << 8,
	kAddedRemote = 1 << 9,
	kConflicted = 1 << 10,
	kLockedLocal = 1 << 11,
	kLockedRemote = 1 << 12,
	kUpdating = 1 << 13,
	kReadOnly = 1 << 14,
	kMetaFile = 1 << 15,
};

class VersionedAsset
{
public:

	VersionedAsset();
	VersionedAsset(const std::string& path);
	VersionedAsset(const std::string& path, int state, const std::string& revision = "");

	int GetState() const;
	void SetState(int newState);
	void AddState(State state);
	void RemoveState(State state);
	
	const std::string& GetPath() const;
	void SetPath(const std::string& path);
	const std::string& GetRevision() const;
	void SetRevision(const std::string& r);

	void Reset();
	bool IsFolder() const;

private:
	int m_State;
	std::string m_Path;
	std::string m_Revision;
};

struct ByPathSorter
{
	bool operator()(const VersionedAsset& a1, const VersionedAsset& a2)
	{
		return a1.GetPath() < a2.GetPath();
	}
};

#include <vector>
typedef std::vector<VersionedAsset> VersionedAssetList;

std::vector<std::string> Paths(const VersionedAssetList& assets);

struct UnityPipe;
UnityPipe& operator<<(UnityPipe& p, const VersionedAsset& v);
UnityPipe& operator>>(UnityPipe& p, VersionedAsset& v);
