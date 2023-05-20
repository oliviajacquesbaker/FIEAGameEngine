/// <summary>
/// Definitions for ReactionAttributed.
/// </summary>

#include "pch.h"
#include "ReactionAttributed.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
    RTTI_DEFINITIONS(ReactionAttributed);

    ReactionAttributed::ReactionAttributed(const std::string& name, std::string subtype) :
        Reaction(ReactionAttributed::TypeIdClass(), name), _subtype{std::move(subtype)}
    {
        Event<EventMessageAttributed>::Subscribe(notificationDelegate);
    }

    ReactionAttributed::ReactionAttributed(RTTI::IdType typeID, const std::string& name, std::string subtype) :
        Reaction(typeID, name), _subtype{ std::move(subtype) }
    {
        Event<EventMessageAttributed>::Subscribe(notificationDelegate);
    }

    ReactionAttributed::~ReactionAttributed() {
        Event<EventMessageAttributed>::Unsubscribe(notificationDelegate);
    }

    gsl::owner<ReactionAttributed*> ReactionAttributed::Clone() const {
        return new ReactionAttributed(*this);
    }

    Vector<Signature> ReactionAttributed::Signatures() {    
        return Vector<Signature> {
            { "subtype"s, Datum::DatumTypes::String, 1, offsetof(ReactionAttributed, _subtype) } 
        };
    }

    void ReactionAttributed::Update(const GameTime& gameTime) {
        ActionList::Update(gameTime);
    }

    void ReactionAttributed::Notify(EventPublisher& eventPublisher) {
        Event<EventMessageAttributed>* eventMessageAttributed = eventPublisher.As<Event<EventMessageAttributed>>();
        assert(eventMessageAttributed);

        const EventMessageAttributed& eventMessage = eventMessageAttributed->Message();
        if (_subtype == eventMessage.Subtype()) {
            CopyInParameters(eventMessage);
            Update(GameState::GetGameTime());
        }
    }

    void ReactionAttributed::CopyInParameters(const EventMessageAttributed& eventMessage) {
        assert(actionsDatum);
        eventMessage.ForEachAuxiliaryAttribute([this, &actionsDatum = actionsDatum](const std::string& key, Datum& value) {
            for (std::size_t i = 0; i < actionsDatum->Size(); ++i) {
                actionsDatum->GetAsTable(i).Append(key).SetFromString(value.ToString());
            }
            return false;
        });
    }

    bool ReactionAttributed::Equals(const RTTI* rhs) const {
        if (rhs == nullptr) return false;

        const ReactionAttributed* other = rhs->As<ReactionAttributed>();
        return (other != nullptr ? *this == *other : false);
    }

}

