#pragma once
#include "document_context.hpp"
#include "references.hpp"
#include "segment.hpp"

namespace persistence
{
    /** Context for de-serializing an object from a JSON DOM document. */
    struct GlobalDeserializerContext : DocumentContext
    {
        using DocumentContext::DocumentContext;

        ReferenceTable<const rapidjson::Value*, std::shared_ptr<void>> references;
    };

    struct DeserializerContext
    {
        DeserializerContext(GlobalDeserializerContext& global)
            : global(global)
            , parent(nullptr)
        {}

        DeserializerContext(DeserializerContext& local, Segment segment)
            : global(local.global)
            , parent(&local)
            , segment(segment)
        {}

        std::shared_ptr<void> get(const rapidjson::Value* pointer) const
        {
            return global.references.get(pointer);
        }

        void put(const rapidjson::Value* pointer, std::shared_ptr<void> object)
        {
            global.references.put(pointer, object);
        }

        rapidjson::Document& document()
        {
            return global.document();
        }

        rapidjson::MemoryPoolAllocator<>& allocator()
        {
            return global.allocator();
        }

        Segments segments() const
        {
            Segments segs;
            const DeserializerContext* ctx = this;
            while (ctx->parent != nullptr) {
                segs.push_back(ctx->segment);
                ctx = ctx->parent;
            };
            return segs;
        }

    private:
        GlobalDeserializerContext& global;
        DeserializerContext* parent;
        Segment segment;
    };
}
