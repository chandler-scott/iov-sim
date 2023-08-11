//
// Generated file, do not edit! Created by opp_msgtool 6.0 from iov_sim/modules/messages/model/ModelBaseMessage.msg.
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
#include "ModelBaseMessage_m.h"

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

Register_Class(ModelBaseMessage)

ModelBaseMessage::ModelBaseMessage(const char *name, short kind) : ::iov_sim::BaseMessage(name, kind)
{
}

ModelBaseMessage::ModelBaseMessage(const ModelBaseMessage& other) : ::iov_sim::BaseMessage(other)
{
    copy(other);
}

ModelBaseMessage::~ModelBaseMessage()
{
}

ModelBaseMessage& ModelBaseMessage::operator=(const ModelBaseMessage& other)
{
    if (this == &other) return *this;
    ::iov_sim::BaseMessage::operator=(other);
    copy(other);
    return *this;
}

void ModelBaseMessage::copy(const ModelBaseMessage& other)
{
    this->data = other.data;
    this->origin = other.origin;
    this->destination = other.destination;
    this->pStateDict = other.pStateDict;
    this->vStateDict = other.vStateDict;
}

void ModelBaseMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::iov_sim::BaseMessage::parsimPack(b);
    doParsimPacking(b,this->data);
    doParsimPacking(b,this->origin);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->pStateDict);
    doParsimPacking(b,this->vStateDict);
}

void ModelBaseMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::iov_sim::BaseMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->data);
    doParsimUnpacking(b,this->origin);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->pStateDict);
    doParsimUnpacking(b,this->vStateDict);
}

const char * ModelBaseMessage::getData() const
{
    return this->data.c_str();
}

void ModelBaseMessage::setData(const char * data)
{
    this->data = data;
}

const char * ModelBaseMessage::getOrigin() const
{
    return this->origin.c_str();
}

void ModelBaseMessage::setOrigin(const char * origin)
{
    this->origin = origin;
}

const char * ModelBaseMessage::getDestination() const
{
    return this->destination.c_str();
}

void ModelBaseMessage::setDestination(const char * destination)
{
    this->destination = destination;
}

const char * ModelBaseMessage::getPStateDict() const
{
    return this->pStateDict.c_str();
}

void ModelBaseMessage::setPStateDict(const char * pStateDict)
{
    this->pStateDict = pStateDict;
}

const char * ModelBaseMessage::getVStateDict() const
{
    return this->vStateDict.c_str();
}

void ModelBaseMessage::setVStateDict(const char * vStateDict)
{
    this->vStateDict = vStateDict;
}

class ModelBaseMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_data,
        FIELD_origin,
        FIELD_destination,
        FIELD_pStateDict,
        FIELD_vStateDict,
    };
  public:
    ModelBaseMessageDescriptor();
    virtual ~ModelBaseMessageDescriptor();

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

Register_ClassDescriptor(ModelBaseMessageDescriptor)

ModelBaseMessageDescriptor::ModelBaseMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(iov_sim::ModelBaseMessage)), "iov_sim::BaseMessage")
{
    propertyNames = nullptr;
}

ModelBaseMessageDescriptor::~ModelBaseMessageDescriptor()
{
    delete[] propertyNames;
}

bool ModelBaseMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ModelBaseMessage *>(obj)!=nullptr;
}

const char **ModelBaseMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ModelBaseMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ModelBaseMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int ModelBaseMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_data
        FD_ISEDITABLE,    // FIELD_origin
        FD_ISEDITABLE,    // FIELD_destination
        FD_ISEDITABLE,    // FIELD_pStateDict
        FD_ISEDITABLE,    // FIELD_vStateDict
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *ModelBaseMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "data",
        "origin",
        "destination",
        "pStateDict",
        "vStateDict",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int ModelBaseMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "data") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "origin") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destination") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "pStateDict") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "vStateDict") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *ModelBaseMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_data
        "string",    // FIELD_origin
        "string",    // FIELD_destination
        "string",    // FIELD_pStateDict
        "string",    // FIELD_vStateDict
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **ModelBaseMessageDescriptor::getFieldPropertyNames(int field) const
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

const char *ModelBaseMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int ModelBaseMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ModelBaseMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ModelBaseMessage'", field);
    }
}

const char *ModelBaseMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ModelBaseMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_data: return oppstring2string(pp->getData());
        case FIELD_origin: return oppstring2string(pp->getOrigin());
        case FIELD_destination: return oppstring2string(pp->getDestination());
        case FIELD_pStateDict: return oppstring2string(pp->getPStateDict());
        case FIELD_vStateDict: return oppstring2string(pp->getVStateDict());
        default: return "";
    }
}

void ModelBaseMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_data: pp->setData((value)); break;
        case FIELD_origin: pp->setOrigin((value)); break;
        case FIELD_destination: pp->setDestination((value)); break;
        case FIELD_pStateDict: pp->setPStateDict((value)); break;
        case FIELD_vStateDict: pp->setVStateDict((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ModelBaseMessage'", field);
    }
}

omnetpp::cValue ModelBaseMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_data: return pp->getData();
        case FIELD_origin: return pp->getOrigin();
        case FIELD_destination: return pp->getDestination();
        case FIELD_pStateDict: return pp->getPStateDict();
        case FIELD_vStateDict: return pp->getVStateDict();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ModelBaseMessage' as cValue -- field index out of range?", field);
    }
}

void ModelBaseMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_data: pp->setData(value.stringValue()); break;
        case FIELD_origin: pp->setOrigin(value.stringValue()); break;
        case FIELD_destination: pp->setDestination(value.stringValue()); break;
        case FIELD_pStateDict: pp->setPStateDict(value.stringValue()); break;
        case FIELD_vStateDict: pp->setVStateDict(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ModelBaseMessage'", field);
    }
}

const char *ModelBaseMessageDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr ModelBaseMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ModelBaseMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelBaseMessage *pp = omnetpp::fromAnyPtr<ModelBaseMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ModelBaseMessage'", field);
    }
}

}  // namespace iov_sim

namespace omnetpp {

}  // namespace omnetpp

