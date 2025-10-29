## GFantasy [ UE 5.4 Team Project]
<img width="1920" height="1200" alt="image" src="https://github.com/user-attachments/assets/1bf725a9-e0a1-4320-80ed-97d01259218e" />

# 1. 프로젝트 개요 

게임 레퍼런스 : FF16 <br>
게임 장르 : ARPG 멀티 게임 <br>
개발 엔진 : UE5.4 소스 엔진 버전 사용 <br>
멀티 여부 : 최대 4인 플레이 가능 <br>
개발 인원 : 4명 <br>
개발 기간 : 25.04.21 ~ 25.06.23 ( 약 2개월 ) <br>
사용 언어 : C++ (80%)/ Blueprint (20%) <br>


# 2. 개발 부분 

2.1 캐릭터 전투 ( GAS 활용 ) 
 #### [ 콤보 공격 ( 클릭하여 코드로 이동 ) ](https://github.com/durung8213/GFantasy/blob/main/Source/GFantasy/Private/GAS/GameplayAbility/GA_AttackCombo.cpp#L99) <br>
 ![G판타지_콤보](https://github.com/user-attachments/assets/df6e95c3-daef-4416-8f8f-0de8ada2fd10) <br>
    : 4단 콤보로 Gameplay Ability를 활용해 구현 <br><br>

   
 #### [ 차징 스킬 ](https://github.com/durung8213/GFantasy/blob/main/Source/GFantasy/Private/GAS/GameplayAbility/GA_AttackCombo.cpp#L412) <br>
 ![G판타지_차징스킬](https://github.com/user-attachments/assets/7b8b3027-f143-4ca7-bc04-1aae3a568b1c) <br>
    : 1단 콤보 이후 꾹 누르고 있으면 차징 스킬 활성화 -> 이후 키 입력을 떼면 AOE 스킬 실행 <br><br>
     
 #### [ 쿨타임 스킬 ](https://github.com/durung8213/GFantasy/blob/main/Source/GFantasy/Private/GAS/GameplayAbility/PlayerSkill/GA_RainAttack.cpp#L98) <br>
![G판타지_쿨타임스킬](https://github.com/user-attachments/assets/954007b9-eacb-4a30-b605-abde6b05373e) <br>
    : GameplayEffect 활용, LineTrace를 이용하여 Target에 AOE 생성하여 범위 스킬 구현 <br><br>
    
#### [ 락온 시스템 ](https://github.com/durung8213/GFantasy/blob/main/Source/GFantasy/Private/Character/GASPlayerCharacter.cpp#L1013) <br>

![락온 자동 공격](https://github.com/user-attachments/assets/fb4f44ac-b7b9-463d-b66c-7b0fea156cde) <br>
: Timer를 활용하여 일정 시간동안 캐릭터 주변의 적을 탐색 <br>
: R 버튼을 눌러 락온 기능 사용 가능 / 해당 적에게 자동 공격되도록 구현 <br><br>

#### [ 투사체 공격 ](https://github.com/durung8213/GFantasy/blob/main/Source/GFantasy/Private/GAS/GameplayAbility/GA_RangedCombo.cpp#L124) <br>
![투사체 자동 유도](https://github.com/user-attachments/assets/f183daae-a380-4921-a401-0cb2def67bcc)<br>
 : 원거리 투사체 공격 구현<br>
 : 감지된 적이 있을때는 무조건 해당 적으로 자동 유도 되도록 구현<br><br>

 #### [ 필살기 공격 ](https://github.com/durung8213/GFantasy/blob/main/Source/GFantasy/Private/GAS/GameplayAbility/PlayerSkill/GA_LimitBreak.cpp#L204) <br>
![필살기](https://github.com/user-attachments/assets/caae5a06-1d32-4a47-b027-b0f329175ce5) <br>
  : 공격 / 피격 당할 때마다 필살 게이지가 증가하도록 구현 <br>
  : AOE 를 활용하여 캐릭터 주변의 적들을 공격. <br><br>
 

2.2 탈 것 시스템

#### [ 탈 것 기능 & 디졸브 효과 ](https://github.com/durung8213/GFantasy/blob/main/Source/GFantasy/Private/Actor/Riding/RidingComponent.cpp#L29) <br>
![탈것](https://github.com/user-attachments/assets/888e1bdc-68fc-440b-b233-b160d4d712ff)<br>

: ActorComponent로 RidingComponent를 생성하여 Character에 부착하여 사용 <br>
: 머테리얼을 활용하여 소환 / 제거 될 때 자연스럽게 보이도록 구현 <br><br>




# 3.  [ 영상 링크 (클릭) ] (https://youtu.be/Nfqxik0QWBo) <br><br><br>

# 4. [ 포트폴리오 링크 (클릭) ](https://drive.google.com/file/d/1ZnebToaCGHJkmClal0KDEfAmYeGXMyhN/view?usp=drive_link) <br><br><br>

