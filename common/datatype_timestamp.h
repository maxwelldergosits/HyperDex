

#ifndef hyperdex_common_datatype_timestamp_h_
#define hyperdex_common_datatype_timestamp_h_

// HyperDex
#include "namespace.h"
#include "common/datatype_info.h"
enum timestamp_interval{
  SECOND,
  MINUTE,
  HOUR,
  DAY,
  WEEK,
  MONTH
};

BEGIN_HYPERDEX_NAMESPACE




class datatype_timestamp : public datatype_info
{
    public:
        datatype_timestamp(enum timestamp_interval);
        virtual ~datatype_timestamp() throw ();

    public:
        virtual hyperdatatype datatype() const;
        virtual bool validate(const e::slice& value) const;
        virtual bool check_args(const funcall& func) const;
        virtual uint8_t* apply(const e::slice& old_value,
                               const funcall* funcs, size_t funcs_sz,
                               uint8_t* writeto);

    public:
        virtual bool hashable() const;
        virtual uint64_t hash(const e::slice& value);
        virtual bool indexable() const;

    public:
        virtual bool containable() const;
        virtual bool step(const uint8_t** ptr,
                          const uint8_t* end,
                          e::slice* elem);
        virtual uint8_t* write(uint8_t* writeto,
                               const e::slice& elem);
        virtual bool comparable() const;
        virtual int compare(const e::slice& lhs, const e::slice& rhs);
        virtual compares_less compare_less();
    private:
      enum timestamp_interval interval;
};

END_HYPERDEX_NAMESPACE

#endif // hyperdex_common_datatype_string_h_
