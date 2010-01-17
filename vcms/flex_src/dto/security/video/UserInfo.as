package dto.security.video
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.UserInfo")]
	public class UserInfo
	{
		public var userId:int;
		public var userName:String;
		public var password:String;
		public var sex:Boolean;
		
		public function UserInfo()
		{
			//TODO: implement function
		}

	}
}