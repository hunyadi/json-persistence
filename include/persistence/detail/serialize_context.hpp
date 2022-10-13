#pragma once
#include "context.hpp"
#include "references.hpp"
#include "segment.hpp"

namespace persistence
{
    struct GlobalSerializerContext : GlobalContext
    {
        using GlobalContext::GlobalContext;

        ReferenceTable<std::shared_ptr<void>, Segments> references;
    };

    struct SerializerContext
    {
        SerializerContext(GlobalSerializerContext& global)
            : global(global)
            , parent(nullptr)
        {}

        SerializerContext(SerializerContext& local, Segment segment)
            : global(local.global)
            , parent(&local)
            , segment(segment)
        {}

        Segments get(std::shared_ptr<void> pointer) const
        {
            return global.references.get(pointer);
        }

        void put(std::shared_ptr<void> pointer)
        {
            global.references.put(pointer, segments());
        }

        rapidjson::MemoryPoolAllocator<>& allocator()
        {
            return global.allocator();
        }

        std::vector<Segment> segments() const
        {
            std::vector<Segment> segs;
            const SerializerContext* ctx = this;
            while (ctx->parent != nullptr) {
                segs.push_back(ctx->segment);
                ctx = ctx->parent;
            };
            return segs;
        }

    private:
        GlobalSerializerContext& global;
        SerializerContext* parent;
        Segment segment;
    };
}
