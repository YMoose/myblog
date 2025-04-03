# Snort3
## StreamSplitter
流量预处理器
Protocol Aware Flushing (PAF).
Reassembling the accumulated data and releasing the associated buffers is called flushing
```C
class SO_PUBLIC StreamSplitter
{
public:
    virtual ~StreamSplitter() = default;

    enum Status
    {
        ABORT,   // non-paf operation
        START,   // internal use only
        SEARCH,  // searching for next flush point
        FLUSH,   // flush at given offset
        LIMIT,   // flush to given offset upon reaching paf_max
        SKIP,    // skip ahead to given offset
        LIMITED, // previously did limit flush
        STOP     // stop paf scan loop
    };

    // scan(), finish(), reassemble() are called in this order:
    // (scan (reassemble)*)* finish (reassemble)*

    virtual Status scan(
        Packet*,
        const uint8_t* data,   // in order segment data as it arrives
        uint32_t len,          // length of data
        uint32_t flags,        // packet flags indicating direction of data
        uint32_t* fp           // flush point (offset) relative to data
        ) = 0;

    // finish indicates end of scanning
    // return false to discard any unflushed data
    virtual bool finish(Flow*) { return true; }
    virtual bool init_partial_flush(Flow*) { return false; }

    // the last call to reassemble() will be made with len == 0 if
    // finish() returned true as an opportunity for a final flush
    virtual const StreamBuffer reassemble(
        Flow*,
        unsigned total,        // total amount to flush (sum of iterations)
        unsigned offset,       // data offset from start of reassembly
        const uint8_t* data,   // data to reassemble
        unsigned len,          // length of data to process this iteration
        uint32_t flags,        // packet flags indicating pdu head and/or tail
        unsigned& copied       // actual data copied (1 <= copied <= len)
        );

    virtual bool sync_on_start() const { return false; }
    virtual bool is_paf() { return false; }
    virtual unsigned max(Flow* = nullptr);
    virtual void go_away() { delete this; }

    bool to_server() { return c2s; }
    bool to_client() { return !c2s; }

protected:
    StreamSplitter(bool b) : c2s(b) { }
    uint16_t get_flush_bucket_size();
    unsigned bytes_scanned = 0;

private:
    const bool c2s;
};
```
## Inspector
```C
class SO_PUBLIC Inspector
{
public:
    // main thread functions
    virtual ~Inspector();

    Inspector(const Inspector&) = delete;
    Inspector& operator=(const Inspector&) = delete;

    // access external dependencies here
    // return verification status
    virtual bool configure(SnortConfig*) { return true; }

    // cleanup for inspector instance removal from the running configuration
    // this is only called for inspectors in the default inspection policy that
    // were present in the prior snort configuration and were removed in the snort
    // configuration that is being loaded during a reload_config command
    virtual void tear_down(SnortConfig*) { }

    // called on controls after everything is configured
    // return true if there is nothing to do ever based on config
    virtual bool disable(SnortConfig*) { return false; }

    virtual void show(const SnortConfig*) const { }

    // Specific to Binders to notify them of an inspector being removed from the policy
    // FIXIT-L Probably shouldn't be part of the base Inspector class
    virtual void remove_inspector_binding(SnortConfig*, const char*) { }

    // packet thread functions
    // tinit, tterm called on default policy instance only
    virtual void tinit() { }   // allocate configurable thread local
    virtual void tterm() { }   // purge only, deallocate via api

    // screen incoming packets; only liked packets go to eval
    // default filter is per api proto / paf
    virtual bool likes(Packet*);

    // clear is a bookend to eval() for the active service inspector
    // clear is called when Snort is done with the previously eval'd
    // packet to release any thread-local or flow-based data
    virtual void eval(Packet*) = 0;
    virtual void clear(Packet*) { }

    // framework support
    unsigned get_ref(unsigned i) { return ref_count[i]; }
    void set_ref(unsigned i, unsigned r) { ref_count[i] = r; }

    void add_ref();
    void rem_ref();

    // Reference counts for the inspector that are not thread specific
    void add_global_ref();
    void rem_global_ref();

    bool is_inactive();

    void set_service(SnortProtocolId snort_protocol_id_param)
    { snort_protocol_id = snort_protocol_id_param; }

    SnortProtocolId get_service() const { return snort_protocol_id; }

    // for well known buffers
    // well known buffers may be included among generic below,
    // but they must be accessible from here
    virtual bool get_buf(InspectionBuffer::Type, Packet*, InspectionBuffer&)
    { return false; }

    // for generic buffers
    // key is listed in api buffers
    // id-1 is zero based index into buffers array
    unsigned get_buf_id(const char* key);
    virtual bool get_buf(const char* key, Packet*, InspectionBuffer&);
    virtual bool get_buf(unsigned /*id*/, Packet*, InspectionBuffer&)
    { return false; }

    virtual bool get_fp_buf(InspectionBuffer::Type ibt, Packet* p, InspectionBuffer& bf)
    { return get_buf(ibt, p, bf); }

    // IT_SERVICE only
    virtual class StreamSplitter* get_splitter(bool to_server);

    void set_api(const InspectApi* p)
    { api = p; }

    const InspectApi* get_api()
    { return api; }

    const char* get_name() const;

    void set_alias_name(const char* name)
    { alias_name = name; }

    const char* get_alias_name() const
    { return alias_name; }

    virtual bool is_control_channel() const
    { return false; }

    virtual bool can_carve_files() const
    { return false; }

    virtual bool can_start_tls() const
    { return false; }

    void allocate_thread_storage();
    void set_thread_specific_data(void*);
    void* get_thread_specific_data() const;
    void copy_thread_storage(Inspector*);

    virtual void install_reload_handler(SnortConfig*)
    { }

public:
    static THREAD_LOCAL unsigned slot;

protected:
    // main thread functions
    Inspector();  // internal init only at this point

private:
    const InspectApi* api = nullptr;
    std::shared_ptr<ThreadSpecificData> thread_specific_data;
    std::atomic_uint* ref_count;
    SnortProtocolId snort_protocol_id = 0;
    // FIXIT-E Use std::string to avoid storing a pointer to external std::string buffers
    const char* alias_name = nullptr;
};
```