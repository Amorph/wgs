#pragma once

#include "wgscommon/types.h"

#include <string>
#include <list>
#include <map>

class WGSEUnitPinLink;
class WGSEUnitPin;
class WGSEUnit;

using WGSEUnitName = std::string;
using WGSETypeName = std::string;
using WGSEPinList = std::list<WGSEUnitPin*>;
using WGSEUnitList = std::list<WGSEUnit*>;
using WGSEUnitPinLinkList = std::list<WGSEUnitPinLink*>;

struct WGSEUnitTypeEditorData
{
	wgs_vec4i32 pin_color = { 0, 0, 255, 255 };
	wgs_bool	bold_link = false;
	void*		renderer = nullptr;
};

class WGSEUnitType
{
	friend class WGSEditor;
public:
	WGSEUnitType(const WGSETypeName& name) : name_(name) {};
	const WGSETypeName& name() { return name_; }
	WGSEUnitTypeEditorData& ed() { return ed_; }
protected:

	WGSETypeName name_;
	WGSEUnitTypeEditorData ed_;
};

class WGSEDiagramNode
{
public:
	virtual wgs_uint32 getNodeType() = 0;
};

struct WGSEUnitPinEditorData
{
	wgs_vec4i32 connector_screen_pos;
	wgs_vec2i32 link_pos;
	wgs_bool	has_links;
};

class WGSEUnitPin : public WGSEDiagramNode
{
	friend class WGSEditor;
public:
	static const wgs_uint32 TYPE = 0x0010;
public:
	WGSEUnitPin(const WGSEUnitName& name, WGSEUnitType* type) : name_(name), type_(type) {}
	const WGSEUnitName&		name()	{ return name_; }
	WGSEUnitType*			type()	{ return type_; }
	WGSEUnitPinEditorData&	ed()	{ return ed_; }

	wgs_uint32 getNodeType() override { return TYPE; }
protected:
	WGSEUnitName			name_;
	WGSEUnitType*			type_;

	WGSEUnitPinEditorData	ed_;
};

class WGSEUnitPinLink : public WGSEDiagramNode
{
public:
	static const wgs_uint32 TYPE = 0x0020;
public:
	WGSEUnitPinLink(WGSEUnitPin* source, WGSEUnitPin* target) : source_(source), target_(target) {}

	WGSEUnitPin* source() { return source_; }
	WGSEUnitPin* target() { return target_; }

	wgs_uint32 getNodeType() override { return TYPE; }
protected:
	WGSEUnitPin* source_;
	WGSEUnitPin* target_;
};


struct WGSEUnitEditorData
{
	wgs_uint32 id = 0;
	wgs_vec2i32 pos = { 0, 0 };
	wgs_vec2i32 size = { 0, 0 };

	wgs_vec4i32 screen_pos;
};

class WGSEUnit : public WGSEDiagramNode
{
	friend class WGSEditor;
public:
	static const wgs_uint32 TYPE = 0x0030;
public:
	WGSEUnit(const WGSEUnitName& name);

	void pure(bool pure) { pure_ = pure; }
	bool pure() { return pure_; }

	const WGSEUnitName& name() { return name_; }
	WGSEUnitEditorData& ed() { return ed_; }

	wgs_uint32 getNodeType() override { return TYPE; }
protected:
	WGSEUnitName		name_;
	WGSEPinList			inputs_;
	WGSEPinList			outputs_;
	bool				pure_;

	WGSEUnitEditorData	ed_;
};

class WGSEUnitReroute : public WGSEUnit
{
public:
	static const wgs_uint32 TYPE = 0x0031;
public:
	WGSEUnitReroute();

	wgs_uint32 getNodeType() override { return TYPE; }
protected:

};

struct WGSEDiagramEditorData
{
	wgs_uint32 last_id = 1;
};

class WGSEDiagram
{
	friend class WGSEditor;
public:
	WGSEDiagram();
	void addUnit(WGSEUnit* unit) { units_.push_back(unit); }
	void addLink(WGSEUnitPinLink* link) { links_.push_back(link); }
protected:
	WGSEUnitList units_;
	WGSEUnitPinLinkList links_;
	WGSEDiagramEditorData ed_;
};

class WGSETypeRegistry
{
	using Container = std::map<WGSETypeName, WGSEUnitType*>;
public:
	WGSETypeRegistry();
	~WGSETypeRegistry();

	WGSEUnitType* get(const WGSETypeName& type);

protected:
	Container data_;
};

