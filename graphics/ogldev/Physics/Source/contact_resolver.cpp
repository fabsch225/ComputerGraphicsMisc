/*

        Copyright 2024 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <assert.h>
#include <algorithm>

#include "contact_resolver.h"

namespace OgldevPhysics
{

void ParticleContact::Resolve(float dt)
{
    ResolveVelocity(dt);
    ResolveInterpenetration(dt);
}


float ParticleContact::CalcSeparatingVelocity() const
{
    Vector3f RelVelocity = m_pParticles[0]->GetVelocity();

    if (m_pParticles[1]) {
        RelVelocity -= m_pParticles[1]->GetVelocity();
    }

    float res = RelVelocity.Dot(m_contactNormal);

    return res;
}


void ParticleContact::ResolveInterpenetration(float dt)
{
    if (m_penetration > 0.0f) {
        float TotalReciprocalMass = CalcTotalReciprocalMass();

        if (TotalReciprocalMass > 0.0f) {

            Vector3f MovePerReciprocalMass = m_contactNormal * (m_penetration / TotalReciprocalMass);
            m_particleMovement[0] = MovePerReciprocalMass * m_pParticles[0]->GetReciprocalMass();

            if (m_pParticles[1]) {
                m_particleMovement[1] = MovePerReciprocalMass * -m_pParticles[1]->GetReciprocalMass();
            } else {
                m_particleMovement[1].SetAll(0.0f);
            }

            Vector3f NewPos = m_pParticles[0]->GetPosition() + m_particleMovement[0];
            m_pParticles[0]->SetPosition(NewPos);

            if (m_pParticles[1]) {
                NewPos = m_pParticles[1]->GetPosition() + m_particleMovement[1];
                m_pParticles[1]->SetPosition(NewPos);
            }
        }
    }
}


void ParticleContact::ResolveVelocity(float dt)
{
    float SeparatingVelocity = CalcSeparatingVelocity();

    if (SeparatingVelocity <= 0.0f) {
        float NewSepVelocity = -SeparatingVelocity * m_restitution;

        float SepVelocityCausedByAccel = CalcSepVelocityCausedByAccel(dt);

        if (SepVelocityCausedByAccel < 0) {
            NewSepVelocity += m_restitution * SepVelocityCausedByAccel;

            NewSepVelocity = std::max(0.0f, NewSepVelocity);
        }

        float DeltaVelocity = NewSepVelocity - SeparatingVelocity;

        float TotalReciprocalMass = CalcTotalReciprocalMass();

        if (TotalReciprocalMass > 0.0f) {
            float Impluse = DeltaVelocity / TotalReciprocalMass;

            Vector3f ImpulsePerMass = m_contactNormal * Impluse;

            Vector3f NewVelocity = m_pParticles[0]->GetVelocity() + ImpulsePerMass * m_pParticles[0]->GetReciprocalMass();

            m_pParticles[0]->SetVelocity(NewVelocity);

            if (m_pParticles[1]) {
                NewVelocity = m_pParticles[1]->GetVelocity() + ImpulsePerMass * -m_pParticles[1]->GetReciprocalMass();

                m_pParticles[1]->SetVelocity(NewVelocity);
            }
        }
    }
}


float ParticleContact::CalcSepVelocityCausedByAccel(float dt)
{
    Vector3f VelocityCausedByAccel = m_pParticles[0]->GetAcceleration();

    if (m_pParticles[1]) {
        VelocityCausedByAccel -= m_pParticles[1]->GetAcceleration();
    }

    float SepVelocityCausedByAccel = VelocityCausedByAccel.Dot(m_contactNormal) * dt;

    return SepVelocityCausedByAccel;
}


float ParticleContact::CalcTotalReciprocalMass()
{
    float TotalReciprocalMass = m_pParticles[0]->GetReciprocalMass();

    if (m_pParticles[1]) {
        TotalReciprocalMass += m_pParticles[1]->GetReciprocalMass();
    }

    return TotalReciprocalMass;
}


void ParticleContactResolver::ResolveContacts(std::vector<ParticleContact>& ContactArray, uint NumContacts, float dt)
{
    int IterationsUsed = 0;

    while (IterationsUsed < m_iterations) {
        int Index = FindContactWithLargestClosingVelocity(ContactArray, NumContacts);

        if (Index == NumContacts) {
            break;
        }

        ContactArray[Index].Resolve(dt);

        // Update the interpenetrations for all particles
        Vector3f* Move = &(ContactArray[Index].m_particleMovement[0]);

        for (uint i = 0; i < NumContacts; i++) {
            if (ContactArray[i].m_pParticles[0] == ContactArray[Index].m_pParticles[0]) {
                ContactArray[i].m_penetration -= Move[0].Dot(ContactArray[i].m_contactNormal);
            } else if (ContactArray[i].m_pParticles[0] == ContactArray[Index].m_pParticles[1]) {
                ContactArray[i].m_penetration -= Move[1].Dot( ContactArray[i].m_contactNormal);
            }

            if (ContactArray[i].m_pParticles[1]) {
                if (ContactArray[i].m_pParticles[1] == ContactArray[Index].m_pParticles[0]) {
                    ContactArray[i].m_penetration += Move[0].Dot(ContactArray[i].m_contactNormal);
                } else if (ContactArray[i].m_pParticles[1] == ContactArray[Index].m_pParticles[1]) {
                    ContactArray[i].m_penetration += Move[1].Dot(ContactArray[i].m_contactNormal);
                }
            }
        }

        IterationsUsed++;
    }
}


int ParticleContactResolver::FindContactWithLargestClosingVelocity(std::vector<ParticleContact>& ContactArray, uint NumContacts)
{
    float MaxSepVelocity = FLT_MAX;
    uint MaxIndex = NumContacts;

    assert(NumContacts <= MaxIndex);

    for (uint i = 0; i < NumContacts; i++) {
        float SepVelocity = ContactArray[i].CalcSeparatingVelocity();

        if ((SepVelocity < MaxSepVelocity) && ((SepVelocity < 0.0f) || (ContactArray[i].GetPenetration() > 0.0f))) {
            MaxSepVelocity = SepVelocity;
            MaxIndex = i;
        }
    }

    return MaxIndex;
}


float ParticleLink::GetLength() const
{
    assert(m_pParticles[0]);
    assert(m_pParticles[1]);

    Vector3f RelativePos = m_pParticles[0]->GetPosition() - m_pParticles[1]->GetPosition();

    float Len = RelativePos.Length();

    return Len;
}


int ParticleCable::AddContact(std::vector<ParticleContact>& Contacts, int StartIndex) const
{
    float Length = GetLength();

    int Ret = 0;

    if (Length >= m_maxLength) {
        Contacts[StartIndex].m_pParticles[0] = m_pParticles[0];
        Contacts[StartIndex].m_pParticles[1] = m_pParticles[1];

        Vector3f Normal = m_pParticles[1]->GetPosition() - m_pParticles[0]->GetPosition();

        Normal.Normalize();

        Contacts[StartIndex].SetContactNormal(Normal);
        Contacts[StartIndex].SetPenetration(Length - m_maxLength);
        Contacts[StartIndex].SetRestitution(m_restituion);

        Ret = 1;
    }

    return Ret;
}


int ParticleRod::AddContact(std::vector<ParticleContact>& Contacts, int StartIndex) const
{
    float CurLength = GetLength();

    int Ret = 0;

    if (CurLength >= m_len) {
        Contacts[StartIndex].m_pParticles[0] = m_pParticles[0];
        Contacts[StartIndex].m_pParticles[1] = m_pParticles[1];

        Vector3f Normal = m_pParticles[1]->GetPosition() - m_pParticles[0]->GetPosition();

        Normal.Normalize();

        if (CurLength > m_len) {
            Contacts[StartIndex].SetContactNormal(Normal);
            Contacts[StartIndex].SetPenetration(CurLength - m_len);
        }
        else {
            Contacts[StartIndex].SetContactNormal(Normal *= -1.0f);
            Contacts[StartIndex].SetPenetration(m_len - CurLength);
        }

        Contacts[StartIndex].SetRestitution(0.0f);

        Ret = 1;
    }

    return Ret;
}


float ParticleConstraint::GetCurLength() const
{
    Vector3f RelativePos = m_pParticle->GetPosition() - m_anchor;
    return RelativePos.Length();
}


int ParticleCableConstraint::AddContact(std::vector<ParticleContact>& Contacts, int StartIndex) const
{
    float Len = GetCurLength();

    if (Len < m_maxLength) {
        return 0;
    }

    Contacts[StartIndex].m_pParticles[0] = m_pParticle;
    Contacts[StartIndex].m_pParticles[1] = NULL;

    Vector3f Normal = m_anchor - m_pParticle->GetPosition();
    Normal.Normalize();
    Contacts[StartIndex].SetContactNormal(Normal);

    Contacts[StartIndex].SetPenetration(Len - m_maxLength);
    Contacts[StartIndex].SetRestitution(m_restitution);

    return 1;
}


void GroundContacts::Init(std::vector<Particle*>* pParticles)
{
    m_pParticles = pParticles;
}


int GroundContacts::AddContact(std::vector<ParticleContact>& Contacts, int StartIndex) const
{
    int Count = 0;
    int Limit = (int)Contacts.size() - StartIndex;

    Vector3f Up(0.0f, 1.0f, 0.0f);

    for (std::vector<Particle*>::iterator it = m_pParticles->begin(); it != m_pParticles->end(); it++) {

        float y = (*it)->GetPosition().y;

        if (y < 0.0f) {
            Contacts[StartIndex].SetContactNormal(Up);
            Contacts[StartIndex].m_pParticles[0] = *it;
            Contacts[StartIndex].m_pParticles[1] = NULL;
            Contacts[StartIndex].SetPenetration(-y);
            Contacts[StartIndex].SetRestitution(0.2f);
            StartIndex++;
            Count++;
        }

        if (Count >= Limit) {
            break;
        }
    }

    return Count;
}

}