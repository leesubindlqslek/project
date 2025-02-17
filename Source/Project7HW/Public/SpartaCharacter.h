#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class PROJECT7HW_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 기본 생성자
	ASpartaCharacter();

	// 카메라 및 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// 체력 UI 위젯 컴포넌트 (예: 체력 표시 UI)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	// 체력 관련 함수들
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	// 데미지 처리 및 체력 감소
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	// 최대 체력 (디폴트 100)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;
	  
	// 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	// 초기화 및 플레이 시작 시 처리
	virtual void BeginPlay() override;

	// 플레이어 입력 처리
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 점프, 이동 등
	UFUNCTION()
	void Move(const FInputActionValue& value);

	UFUNCTION()
	void StartJump(const FInputActionValue& value);

	UFUNCTION()
	void StopJump(const FInputActionValue& value);

	UFUNCTION()
	void Look(const FInputActionValue& value);

	UFUNCTION()
	void StartSprint(const FInputActionValue& value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	// 사망 처리
	void OnDeath();

	// 체력 UI 업데이트
	void UpdateOverheadHP();

private:
	// 스프린트 관련 변수
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
};
