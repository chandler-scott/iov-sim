//
// Generated file, do not edit! Created by opp_msgtool 6.0 from iov_sim/modules/messages/cluster/ClusterBaseMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "ClusterBaseMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace iov_sim {

Register_Class(ClusterBaseMessage)

ClusterBaseMessage::ClusterBaseMessage(const char *name, short kind) : ::iov_sim::BaseMessage(name, kind)
{
}

ClusterBaseMessage::ClusterBaseMessage(const ClusterBaseMessage& other) : ::iov_sim::BaseMessage(other)
{
    copy(other);
}

ClusterBaseMessage::~ClusterBaseMessage()
{
}

ClusterBaseMessage& ClusterBaseMessage::operator=(const ClusterBaseMessage& other)
{
    if (this == &other) return *this;
    ::iov_sim::BaseMessage::operator=(other);
    copy(other);
    return *this;
}

void ClusterBaseMessage::copy(const ClusterBaseMessage& other)
{
    this->origin = other.origin;
    this->clusterId = other.clusterId;
    this->timestamp = other.timestamp;
}

void ClusterBaseMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::iov_sim::BaseMessage::parsimPack(b);
    doParsimPacking(b,this->origin);
    doParsimPacking(b,this->clusterId);
    doParsimPacking(b,this->timestamp);
}

void ClusterBaseMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::iov_sim::BaseMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->origin);
    doParsimUnpacking(b,this->clusterId);
    doParsimUnpacking(b,this->timestamp);
}

const char * ClusterBaseMessage::getOrigin() const
{
    return this->origin.c_str();
}

void ClusterBaseMessage::setOrigin(const char * origin)
{
    this->origin = origin;
}

const char * ClusterBaseMessage::getClusterId() const
{
    return this->clusterId.c_str();
}

void ClusterBaseMessage::setClusterId(const char * clusterId)
{
    this->clusterId = clusterId;
}

double ClusterBaseMessage::getTimestamp() const
{
    return this->timestamp;
}

void ClusterBaseMessage::setTimestamp(double timestamp)
{
    this->timestamp = timestamp;
}

class ClusterBaseMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_origin,
        FIELD_clusterId,
        FIELD_timestamp,
    };
  public:
    ClusterBaseMessageDescriptor();
    virtual ~ClusterBaseMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ClusterBaseMessageDescriptor)

ClusterBaseMessageDescriptor::ClusterBaseMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(iov_sim::ClusterBaseMessage)), "iov_sim::BaseMessage")
{
    propertyNames = nullptr;
}

ClusterBaseMessageDescriptor::~ClusterBaseMessageDescriptor()
{
    delete[] propertyNames;
}

bool ClusterBaseMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ClusterBaseMessage *>(obj)!=nullptr;
}

const char **ClusterBaseMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ClusterBaseMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ClusterBaseMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int ClusterBaseMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_origin
        FD_ISEDITABLE,    // FIELD_clusterId
        FD_ISEDITABLE,    // FIELD_timestamp
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *ClusterBaseMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "origin",
        "clusterId",
        "timestamp",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int ClusterBaseMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "origin") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "clusterId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "timestamp") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *ClusterBaseMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_origin
        "string",    // FIELD_clusterId
        "double",    // FIELD_timestamp
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **ClusterBaseMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ClusterBaseMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ClusterBaseMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ClusterBaseMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ClusterBaseMessage'", field);
    }
}

const char *ClusterBaseMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ClusterBaseMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_origin: return oppstring2string(pp->getOrigin());
        case FIELD_clusterId: return oppstring2string(pp->getClusterId());
        case FIELD_timestamp: return double2string(pp->getTimestamp());
        default: return "";
    }
}

void ClusterBaseMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_origin: pp->setOrigin((value)); break;
        case FIELD_clusterId: pp->setClusterId((value)); break;
        case FIELD_timestamp: pp->setTimestamp(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ClusterBaseMessage'", field);
    }
}

omnetpp::cValue ClusterBaseMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_origin: return pp->getOrigin();
        case FIELD_clusterId: return pp->getClusterId();
        case FIELD_timestamp: return pp->getTimestamp();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ClusterBaseMessage' as cValue -- field index out of range?", field);
    }
}

void ClusterBaseMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_origin: pp->setOrigin(value.stringValue()); break;
        case FIELD_clusterId: pp->setClusterId(value.stringValue()); break;
        case FIELD_timestamp: pp->setTimestamp(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ClusterBaseMessage'", field);
    }
}

const char *ClusterBaseMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ClusterBaseMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ClusterBaseMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ClusterBaseMessage *pp = omnetpp::fromAnyPtr<ClusterBaseMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ClusterBaseMessage'", field);
    }
}

}  // namespace iov_sim

namespace omnetpp {

}  // namespace omnetpp

